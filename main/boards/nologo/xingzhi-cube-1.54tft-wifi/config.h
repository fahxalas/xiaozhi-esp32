
#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

#define AUDIO_INPUT_SAMPLE_RATE   16000
#define AUDIO_OUTPUT_SAMPLE_RATE  24000

// --- MICRÓFONO (I2S Directo / PDM) ---
#define AUDIO_I2S_MIC_GPIO_WS   GPIO_NUM_42
#define AUDIO_I2S_MIC_GPIO_SCK  GPIO_NUM_41
#define AUDIO_I2S_MIC_GPIO_DIN  GPIO_NUM_2

// --- PARLANTE (I2S DAC / Amplificador) ---
#define AUDIO_I2S_SPK_GPIO_DOUT GPIO_NUM_7
#define AUDIO_I2S_SPK_GPIO_BCLK GPIO_NUM_5
#define AUDIO_I2S_SPK_GPIO_LRCK GPIO_NUM_6

// --- BOTONES ---
#define BOOT_BUTTON_GPIO        GPIO_NUM_0
#define VOLUME_UP_BUTTON_GPIO   GPIO_NUM_NC // Desactivado si no existe físico
#define VOLUME_DOWN_BUTTON_GPIO GPIO_NUM_NC

// --- PANTALLA TFT 1.54" (SPI Estándar ESP32-S3) ---
#define DISPLAY_SDA GPIO_NUM_11 // MOSI (Datos)
#define DISPLAY_SCL GPIO_NUM_12 // SCLK (Reloj)
#define DISPLAY_DC  GPIO_NUM_13 // Data / Command
#define DISPLAY_CS  GPIO_NUM_10 // Chip Select
#define DISPLAY_RES GPIO_NUM_9  // Reset
#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_14 // Control de Luz

#define DISPLAY_WIDTH   240
#define DISPLAY_HEIGHT  240
#define DISPLAY_SWAP_XY  false
#define DISPLAY_MIRROR_X false
#define DISPLAY_MIRROR_Y false
#define BACKLIGHT_INVERT false
#define DISPLAY_OFFSET_X  0
#define DISPLAY_OFFSET_Y  0
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false

#endif // _BOARD_CONFIG_H_
