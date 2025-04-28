#include "ws2812.h"
#include <math.h>

#define RESOLUTION_HZ   10000000
#define ONE_US          (RESOLUTION_HZ/1000000)

static rmt_symbol_word_t ws2812_reset = {
    .level0 = 0,
    .duration0 = 25 * ONE_US,
    .level1 = 0,
    .duration1 = 25 * ONE_US,
};

static size_t ws2812_encode(rmt_encoder_t * encoder, rmt_channel_handle_t channel, const void * primary_data, size_t data_size, rmt_encode_state_t * ret_state)
{
    exrmt_encoder_t *exrmt_encoder = __containerof(encoder, exrmt_encoder_t, base);
    rmt_encoder_handle_t bytes_encoder = exrmt_encoder->bytes_encoder;
    rmt_encoder_handle_t copy_encoder = exrmt_encoder->copy_encoder;
    rmt_encode_state_t session_state = RMT_ENCODING_RESET;
    rmt_encode_state_t state = RMT_ENCODING_RESET;
    size_t encoded_symbols = 0;
    switch (exrmt_encoder->state) {
    case 0: // send RGB data
        encoded_symbols += bytes_encoder->encode(bytes_encoder, channel, primary_data, data_size, &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            exrmt_encoder->state = 1; // switch to next state when current encoding session finished
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            state |= RMT_ENCODING_MEM_FULL;
            goto out; // yield if there's no free space for encoding artifacts
        }
    // fall-through
    case 1: // send reset code
        encoded_symbols += copy_encoder->encode(copy_encoder, channel, &ws2812_reset,
                                                sizeof(ws2812_reset), &session_state);
        if (session_state & RMT_ENCODING_COMPLETE) {
            exrmt_encoder->state = RMT_ENCODING_RESET; // back to the initial encoding session
            state |= RMT_ENCODING_COMPLETE;
        }
        if (session_state & RMT_ENCODING_MEM_FULL) {
            state |= RMT_ENCODING_MEM_FULL;
            goto out; // yield if there's no free space for encoding artifacts
        }
    }
out:
    *ret_state = state;
    return encoded_symbols;
}

ws2812_handle_t ws2812_init(gpio_num_t gpio_num, uint32_t led_num)
{
    assert(led_num);
    ws2812_t* ws2812 = malloc(sizeof(ws2812_t));
    assert(ws2812);
    ws2812->grb_buffer = malloc(led_num * 3);
    assert(ws2812->grb_buffer);
    ws2812->led_num = led_num;
    ws2812->channel = exrmt_tx_channel_init(RESOLUTION_HZ, gpio_num);
    rmt_bytes_encoder_config_t bytes_encoder_config = {
        .bit0 = {
            .level0 = 1,
            .duration0 = 0.3 * ONE_US,
            .level1 = 0,
            .duration1 = 0.9 * ONE_US,
        },
        .bit1 = {
            .level0 = 1,
            .duration0 = 0.9 * ONE_US,
            .level1 = 0,
            .duration1 = 0.3 * ONE_US,
        },
        .flags.msb_first = 1
    };
    ws2812->encoder = exrmt_new_encoder(&bytes_encoder_config, ws2812_encode, NULL);
    exrmt_enable(ws2812->channel, true);
    return ws2812;
}

void ws2812_deinit(ws2812_handle_t ws2812)
{
    if(ws2812)
    {
        if(ws2812->grb_buffer)
        {
            free(ws2812->grb_buffer);
        }
        free(ws2812);
        ws2812 = NULL;
    }
}

void ws2812_update_color(ws2812_handle_t ws2812)
{
    assert(ws2812);
    exrmt_transmit(ws2812->channel, ws2812->encoder, ws2812->grb_buffer, ws2812->led_num * 3, 0);
}

void ws2812_set_rgb(ws2812_handle_t ws2812, uint32_t index, uint8_t r, uint8_t g, uint8_t b)
{
    assert(ws2812);
    if(index >= ws2812->led_num)
        return;
    uint32_t start = index * 3;
    ws2812->grb_buffer[start + 0] = g;
    ws2812->grb_buffer[start + 1] = r;
    ws2812->grb_buffer[start + 2] = b;
}

void ws2812_set_rgb_all(ws2812_handle_t ws2812, uint8_t r, uint8_t g, uint8_t b)
{
    assert(ws2812);
    for(int i = 0; i < ws2812->led_num; ++i)
    {
        uint32_t start = i * 3;
        ws2812->grb_buffer[start + 0] = g;
        ws2812->grb_buffer[start + 1] = r;
        ws2812->grb_buffer[start + 2] = b;
    }
}

void ws2812_set_hsv(ws2812_handle_t ws2812, uint32_t index, float h, float s, float v)
{
    assert(ws2812);
    if(index >= ws2812->led_num)
        return;
    float r, g, b;
    ws2812_hsv2rgb(h, s, v, &r, &g, &b);
    uint32_t start = index * 3;
    ws2812->grb_buffer[start + 0] = g;
    ws2812->grb_buffer[start + 1] = r;
    ws2812->grb_buffer[start + 2] = b;
}

void ws2812_set_hsv_all(ws2812_handle_t ws2812, float h, float s, float v)
{
    assert(ws2812);
    float r, g, b;
    ws2812_hsv2rgb(h, s, v, &r, &g, &b);
    for(int i = 0; i < ws2812->led_num; ++i)
    {
        uint32_t start = i * 3;
        ws2812->grb_buffer[start + 0] = g;
        ws2812->grb_buffer[start + 1] = r;
        ws2812->grb_buffer[start + 2] = b;
    }
}

void ws2812_hsv2rgb(float h, float s, float v, float *r, float *g, float *b)
{
    float c = v * s;
    float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
    float m = v - c;

    if(h >= 0 && h < 60)
        *r = c, *g = x, *b = 0;
    else if(h >= 60 && h < 120)
        *r = x, *g = c, *b = 0;
    else if(h >= 120 && h < 180)
        *r = 0, *g = c, *b = x;
    else if(h >= 180 && h < 240)
        *r = 0, *g = x, *b = c;
    else if(h >= 240 && h < 300)
        *r = x, *g = 0, *b = c;
    else
        *r = c, *g = 0, *b = x;

    *r = (*r + m) * 255;
    *g = (*g + m) * 255;
    *b = (*b + m) * 255;
}

void ws2812_rgb2hsv(float r, float g, float b, float *h, float *s, float *v)
{
    r = r / 255.0f;
    g = g / 255.0f;
    b = b / 255.0f;

    float max = fmaxf(r, fmaxf(g, b));
    float min = fminf(r, fminf(g, b));
    float delta = max - min;

    *v = max;

    if(max != 0)
        *s = delta / max;
    else
        *s = 0;

    if(delta == 0)
        *h = 0;
    else if(max == r)
        *h = 60 * (fmod(((g - b) / delta), 6));
    else if(max == g)
        *h = 60 * (((b - r) / delta) + 2);
    else if(max == b)
        *h = 60 * (((r - g) / delta) + 4);

    if(*h < 0)
        *h += 360;
}
