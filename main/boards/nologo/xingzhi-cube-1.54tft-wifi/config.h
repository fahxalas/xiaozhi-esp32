#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

#define AUDIO_INPUT_SAMPLE_RATE   24000
#define AUDIO_OUTPUT_SAMPLE_RATE  24000

// --- AUDIO I2S ---
#define AUDIO_I2S_GPIO_BCLK       GPIO_NUM_9
#define AUDIO_I2S_GPIO_WS         GPIO_NUM_11
#define AUDIO_I2S_GPIO_DOUT       GPIO_NUM_12
#define AUDIO_I2S_GPIO_DIN        GPIO_NUM_10
#define AUDIO_CODEC_PA_PIN        GPIO_NUM_4

// --- ENERGÍA / ALIMENTACIÓN DE PERIFÉRICOS ---
#define BUILTIN_LED_POWER         GPIO_NUM_39

// --- BOTONES FÍSICOS DE TU TARJETA ---
#define BOOT_BUTTON_GPIO          GPIO_NUM_21
#define LEFT_BUTTON_GPIO          GPIO_NUM_0
#define RIGHT_BUTTON_GPIO         GPIO_NUM_47

// --- PINES Y RESOLUCIÓN REAL (ST7789 / 240x240) ---
#define DISPLAY_SDA               GPIO_NUM_15
#define DISPLAY_SCL               GPIO_NUM_16
#define DISPLAY_CS                GPIO_NUM_17
#define DISPLAY_DC                GPIO_NUM_18
#define DISPLAY_RES               GPIO_NUM_14

#define DISPLAY_WIDTH             240
#define DISPLAY_HEIGHT            240
#define DISPLAY_SWAP_XY           false
#define DISPLAY_MIRROR_X          false
#define DISPLAY_MIRROR_Y          false
#define DISPLAY_OFFSET_X          0
#define DISPLAY_OFFSET_Y          0

#define DISPLAY_BACKLIGHT_PIN     GPIO_NUM_13
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false

#endif // _BOARD_CONFIG_H_
