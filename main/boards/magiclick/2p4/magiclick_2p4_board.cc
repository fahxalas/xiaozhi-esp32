#include "wifi_board.h"
#include "codecs/es8311_audio_codec.h"
#include "display/lcd_display.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "power_save_timer.h"
#include <i2c_bus.h>

#include <esp_log.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_lcd_nv3023.h>
#include <driver/gpio.h>

#define TAG "MagiClick2P4Board"

class MagiClick2P4Board : public WifiBoard {
private:
    i2c_bus_handle_t i2c_bus_ = nullptr;
    Es8311AudioCodec* audio_codec_ = nullptr;
    Button main_button_;
    Button left_button_;
    Button right_button_;
    SpiLcdDisplay* display_ = nullptr;
    PowerSaveTimer* power_save_timer_ = nullptr;
    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;
    int volume_ = 70;

    void InitializePower() {
        // Energiza pantalla y perifericos (GPIO 39 activo en BAJO)
        gpio_reset_pin(BUILTIN_LED_POWER);
        gpio_set_direction(BUILTIN_LED_POWER, GPIO_MODE_OUTPUT);
        gpio_set_level(BUILTIN_LED_POWER, 0);
    }

    void InitializeI2c() {
        i2c_config_t i2c_cfg = {
            .mode = I2C_MODE_MASTER,
            .sda_io_num = AUDIO_CODEC_I2C_SDA_PIN,
            .scl_io_num = AUDIO_CODEC_I2C_SCL_PIN,
            .sda_pullup_en = GPIO_PULLUP_ENABLE,
            .scl_pullup_en = GPIO_PULLUP_ENABLE,
            .master = {
                .clk_speed = 100000,
            },
            .clk_flags = 0,
        };
        i2c_bus_ = i2c_bus_create(I2C_NUM_0, &i2c_cfg);
    }

    void InitializePowerSaveTimer() {
        power_save_timer_ = new PowerSaveTimer(240, 60, -1);
        power_save_timer_->OnEnterSleepMode([this]() {
            GetDisplay()->SetPowerSaveMode(true);
            GetBacklight()->SetBrightness(1);
        });
        power_save_timer_->OnExitSleepMode([this]() {
            GetDisplay()->SetPowerSaveMode(false);
            GetBacklight()->RestoreBrightness();
        });
        power_save_timer_->SetEnabled(true);
    }

    void InitializeSpi() {
        spi_bus_config_t buscfg = {};
        buscfg.mosi_io_num = DISPLAY_SDA_PIN;
        buscfg.miso_io_num = GPIO_NUM_NC;
        buscfg.sclk_io_num = DISPLAY_SCL_PIN;
        buscfg.quadwp_io_num = GPIO_NUM_NC;
        buscfg.quadhd_io_num = GPIO_NUM_NC;
        buscfg.max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t);
        ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO));
    }

    void InitializeButtons() {
        main_button_.OnClick([this]() {
            power_save_timer_->WakeUp();
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            app.ToggleChatState();
        });
        main_button_.OnPressDown([this]() {
            power_save_timer_->WakeUp();
            Application::GetInstance().StartListening();
        });
        main_button_.OnPressUp([this]() {
            Application::GetInstance().StopListening();
        });

        left_button_.OnClick([this]() {
            power_save_timer_->WakeUp();
            volume_ -= 10;
            if (volume_ < 0) volume_ = 0;
            if (audio_codec_ != nullptr) {
                audio_codec_->SetOutputVolume(volume_);
            }
        });

        right_button_.OnClick([this]() {
            power_save_timer_->WakeUp();
            volume_ += 10;
            if (volume_ > 100) volume_ = 100;
            if (audio_codec_ != nullptr) {
                audio_codec_->SetOutputVolume(volume_);
            }
        });
    }

    void InitializeNv3023Display() {
        ESP_LOGD(TAG, "Install panel IO");
        esp_lcd_panel_io_spi_config_t io_config = {};
        io_config.cs_gpio_num = DISPLAY_CS_PIN;
        io_config.dc_gpio_num = DISPLAY_DC_PIN;
        io_config.spi_mode = 0;
        io_config.pclk_hz = 40 * 1000 * 1000;
        io_config.trans_queue_depth = 10;
        io_config.lcd_cmd_bits = 8;
        io_config.lcd_param_bits = 8;
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI3_HOST, &io_config, &panel_io_));

        ESP_LOGD(TAG, "Install LCD driver NV3023");
        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = DISPLAY_RST_PIN;
        panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR;
        panel_config.bits_per_pixel = 16;
        ESP_ERROR_CHECK(esp_lcd_new_panel_nv3023(panel_io_, &panel_config, &panel_));

        esp_lcd_panel_reset(panel_);
        esp_lcd_panel_init(panel_);
        esp_lcd_panel_invert_color(panel_, false);
        esp_lcd_panel_swap_xy(panel_, DISPLAY_SWAP_XY);
        esp_lcd_panel_mirror(panel_, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);
        ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_, true));

        display_ = new SpiLcdDisplay(panel_io_, panel_, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y, 
            DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y, DISPLAY_SWAP_XY);
    }

public:
    MagiClick2P4Board() :
        main_button_(MAIN_BUTTON_GPIO),
        left_button_(LEFT_BUTTON_GPIO),
        right_button_(RIGHT_BUTTON_GPIO) {
        
        InitializePower();
        InitializeI2c();

        audio_codec_ = new Es8311AudioCodec(i2c_bus_, AUDIO_CODEC_ES8311_ADDR, AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_MCLK, AUDIO_I2S_GPIO_BCLK, AUDIO_I2S_GPIO_WS, AUDIO_I2S_GPIO_DOUT, AUDIO_I2S_GPIO_DIN,
            AUDIO_CODEC_PA_PIN);

        InitializePowerSaveTimer();
        InitializeSpi();
        InitializeButtons();
        InitializeNv3023Display();
        GetBacklight()->RestoreBrightness();
    }

    virtual AudioCodec* GetAudioCodec() override {
        return audio_codec_;
    }

    virtual Display* GetDisplay() override {
        return display_;
    }
    
    virtual Backlight* GetBacklight() override {
        static PwmBacklight backlight(DISPLAY_BACKLIGHT_PIN, DISPLAY_BACKLIGHT_OUTPUT_INVERT);
        return &backlight;
    }

    virtual void SetPowerSaveLevel(PowerSaveLevel level) override {
        if (level != PowerSaveLevel::LOW_POWER) {
            power_save_timer_->WakeUp();
        }
        WifiBoard::SetPowerSaveLevel(level);
    }
};

DECLARE_BOARD(MagiClick2P4Board);
