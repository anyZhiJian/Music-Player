#pragma once

#include "exrmt.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    rmt_channel_handle_t channel;
    rmt_encoder_handle_t encoder;
    uint32_t led_num;
    uint8_t* grb_buffer;
} ws2812_t;

typedef ws2812_t* ws2812_handle_t;

ws2812_handle_t ws2812_init(gpio_num_t gpio_num, uint32_t led_num);
void ws2812_deinit(ws2812_handle_t ws2812);
void ws2812_update_color(ws2812_handle_t ws2812);
void ws2812_set_rgb(ws2812_handle_t ws2812, uint32_t index, uint8_t r, uint8_t g, uint8_t b);
void ws2812_set_rgb_all(ws2812_handle_t ws2812, uint8_t r, uint8_t g, uint8_t b);
void ws2812_set_hsv(ws2812_handle_t ws2812, uint32_t index, float h, float s, float v);
void ws2812_set_hsv_all(ws2812_handle_t ws2812, float h, float s, float v);
void ws2812_hsv2rgb(float h, float s, float v, float *r, float *g, float *b);
void ws2812_rgb2hsv(float r, float g, float b, float *h, float *s, float *v);

#ifdef __cplusplus
}
#endif