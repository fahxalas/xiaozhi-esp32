#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

#define AUDIO_INPUT_SAMPLE_RATE   16000
#define AUDIO_OUTPUT_SAMPLE_RATE  16000

// --- CODEC DE AUDIO ES8311 (I2C) ---
#define AUDIO_CODEC_I2C_SDA_PIN   GPIO_NUM_2
#define AUDIO_CODEC_I2C_SCL_PIN   GPIO_NUM_1
#define AUDIO_CODEC_ES8311_ADDR   0x18

// --- AUDIO I2S ---
#define AUDIO_I2S_GPIO_MCLK       GPIO_NUM_4
#define AUDIO_I2S_GPIO_BCLK       GPIO_NUM_8
#define AUDIO_I2S_GPIO_WS         GPIO_NUM_6
#define AUDIO_I2S_GPIO_DOUT       GPIO_NUM_5
#define AUDIO_I2S_GPIO_DIN        GPIO_NUM_7
#define AUDIO_CODEC_PA_PIN        GPIO_NUM_9

// --- BOTON FISICO ---
#define BOOT_BUTTON_GPIO          GPIO_NUM_0

// --- PANTALLA ST7789V (240x296) ---
#define DISPLAY_SDA               GPIO_NUM_17
#define DISPLAY_SCL               GPIO_NUM_18
#define DISPLAY_CS                GPIO_NUM_21
#define DISPLAY_DC                GPIO_NUM_16
#define DISPLAY_RES               GPIO_NUM_15

#define DISPLAY_WIDTH             240
#define DISPLAY_HEIGHT            296
#define DISPLAY_SWAP_XY           false
#define DISPLAY_MIRROR_X          false
#define DISPLAY_MIRROR_Y          false
#define DISPLAY_OFFSET_X          0
#define DISPLAY_OFFSET_Y          0

#define DISPLAY_BACKLIGHT_PIN     GPIO_NUM_13
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false

#endif // _BOARD_CONFIG_H_
