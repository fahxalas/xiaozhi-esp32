#include "wifi_board.h"
#include "codecs/no_audio_codec.h"
#include "display/lcd_display.h"
#include "application.h"
#include "button.h"
#include "config.h"

#include <esp_log.h>
#include <driver/gpio.h>
#include <driver/spi_master.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_nv3023.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TAG "SCANNER_PANTALLA"

struct ConfigPrueba {
    int id;
    const char* nombre;
    gpio_num_t mosi;
    gpio_num_t sclk;
    gpio_num_t cs;
    gpio_num_t dc;
    gpio_num_t rst;
    gpio_num_t bl;
    int driver; // 0 = NV3023, 1 = ST7789
};

// Combinaciones clave a probar usando gpio_num_t explícito
const ConfigPrueba LISTA_CONFIGS[] = {
    {1, "MagiClick 2.4 Oficial (NV3023)", GPIO_NUM_15, GPIO_NUM_16, GPIO_NUM_17, GPIO_NUM_18, GPIO_NUM_14, GPIO_NUM_13, 0},
    {2, "Cube / MagiClick 2.5 (NV3023)",   GPIO_NUM_11, GPIO_NUM_12, GPIO_NUM_10, GPIO_NUM_13, GPIO_NUM_9,  GPIO_NUM_14, 0},
    {3, "Cube 1.54 Clasico (ST7789)",      GPIO_NUM_13, GPIO_NUM_12, GPIO_NUM_10, GPIO_NUM_11, GPIO_NUM_9,  GPIO_NUM_14, 1},
    {4, "SPI Nativo ESP32-S3 (ST7789)",    GPIO_NUM_11, GPIO_NUM_12, GPIO_NUM_10, GPIO_NUM_8,  GPIO_NUM_9,  GPIO_NUM_13, 1},
    {5, "S3 Alt Pins (ST7789)",            GPIO_NUM_10, GPIO_NUM_9,  GPIO_NUM_14, GPIO_NUM_8,  GPIO_NUM_18, GPIO_NUM_13, 1}
};

class XINGZHI_CUBE_1_54TFT_WIFI : public WifiBoard {
private:
    Button boot_button_;

    static void PintarPantallaPrueba(esp_lcd_panel_handle_t panel, uint16_t color) {
        uint16_t buffer[128 * 10];
        for (int i = 0; i < 128 * 10; i++) buffer[i] = color;
        for (int y = 0; y < 128; y += 10) {
            esp_lcd_panel_draw_bitmap(panel, 0, y, 128, y + 10, buffer);
        }
    }

    static void TaskEscanner(void* pvParameters) {
        // Enciende alimentacion auxiliar de pantalla y audio (GPIO 39 y 4)
        gpio_reset_pin(GPIO_NUM_39);
        gpio_set_direction(GPIO_NUM_39, GPIO_MODE_OUTPUT);
        gpio_set_level(GPIO_NUM_39, 0);

        gpio_reset_pin(GPIO_NUM_4);
        gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
        gpio_set_level(GPIO_NUM_4, 1);

        int total_configs = sizeof(LISTA_CONFIGS) / sizeof(ConfigPrueba);

        while (true) {
            for (int i = 0; i < total_configs; i++) {
                auto cfg = LISTA_CONFIGS[i];
                ESP_LOGI(TAG, " ");
                ESP_LOGI(TAG, "====================================================");
                ESP_LOGI(TAG, "🔍 PROBANDO CONFIGURACION #%d: [%s]", cfg.id, cfg.nombre);
                ESP_LOGI(TAG, "   MOSI=%d, SCLK=%d, CS=%d, DC=%d, RST=%d, BL=%d", 
                         (int)cfg.mosi, (int)cfg.sclk, (int)cfg.cs, (int)cfg.dc, (int)cfg.rst, (int)cfg.bl);
                ESP_LOGI(TAG, "====================================================");

                // Control del Backlight
                gpio_reset_pin(cfg.bl);
                gpio_set_direction(cfg.bl, GPIO_MODE_OUTPUT);
                gpio_set_level(cfg.bl, 1);

                // Configuracion Bus SPI
                spi_bus_config_t buscfg = {};
                buscfg.mosi_io_num = cfg.mosi;
                buscfg.miso_io_num = GPIO_NUM_NC;
                buscfg.sclk_io_num = cfg.sclk;
                buscfg.quadwp_io_num = GPIO_NUM_NC;
                buscfg.quadhd_io_num = GPIO_NUM_NC;
                buscfg.max_transfer_sz = 128 * 128 * sizeof(uint16_t);

                if (spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO) != ESP_OK) {
                    ESP_LOGE(TAG, "Error iniciando bus SPI");
                    continue;
                }

                esp_lcd_panel_io_handle_t panel_io = nullptr;
                esp_lcd_panel_handle_t panel = nullptr;

                esp_lcd_panel_io_spi_config_t io_config = {};
                io_config.cs_gpio_num = cfg.cs;
                io_config.dc_gpio_num = cfg.dc;
                io_config.spi_mode = 0;
                io_config.pclk_hz = 20 * 1000 * 1000;
                io_config.trans_queue_depth = 10;
                io_config.lcd_cmd_bits = 8;
                io_config.lcd_param_bits = 8;

                if (esp_lcd_new_panel_io_spi(SPI3_HOST, &io_config, &panel_io) == ESP_OK) {
                    esp_lcd_panel_dev_config_t panel_config = {};
                    panel_config.reset_gpio_num = cfg.rst;
                    panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR;
                    panel_config.bits_per_pixel = 16;

                    esp_err_t ret = ESP_FAIL;
                    if (cfg.driver == 0) {
                        ret = esp_lcd_new_panel_nv3023(panel_io, &panel_config, &panel);
                    } else {
                        ret = esp_lcd_new_panel_st7789(panel_io, &panel_config, &panel);
                    }

                    if (ret == ESP_OK) {
                        esp_lcd_panel_reset(panel);
                        esp_lcd_panel_init(panel);
                        esp_lcd_panel_disp_on_off(panel, true);

                        // Pruebas de color
                        ESP_LOGI(TAG, "--> Enviando ROJO");
                        PintarPantallaPrueba(panel, 0xF800);
                        vTaskDelay(pdMS_TO_TICKS(1000));

                        ESP_LOGI(TAG, "--> Enviando VERDE");
                        PintarPantallaPrueba(panel, 0x07E0);
                        vTaskDelay(pdMS_TO_TICKS(1000));

                        ESP_LOGI(TAG, "--> Enviando AZUL");
                        PintarPantallaPrueba(panel, 0x001F);
                        vTaskDelay(pdMS_TO_TICKS(1000));

                        esp_lcd_panel_del(panel);
                    }
                    esp_lcd_panel_io_del(panel_io);
                }
                spi_bus_free(SPI3_HOST);
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }
    }

public:
    XINGZHI_CUBE_1_54TFT_WIFI() : boot_button_(BOOT_BUTTON_GPIO) {
        xTaskCreate(TaskEscanner, "TaskEscanner", 4096, NULL, 5, NULL);
    }

    virtual AudioCodec* GetAudioCodec() override {
        static NoAudioCodecSimplex audio_codec(24000, 24000,
            GPIO_NUM_9, GPIO_NUM_11, GPIO_NUM_12, GPIO_NUM_9, GPIO_NUM_11, GPIO_NUM_10);
        return &audio_codec;
    }

    virtual Display* GetDisplay() override { return nullptr; }
    virtual Backlight* GetBacklight() override {
        static PwmBacklight backlight(GPIO_NUM_13, false);
        return &backlight;
    }
};

DECLARE_BOARD(XINGZHI_CUBE_1_54TFT_WIFI);
