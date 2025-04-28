#pragma once

#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

// 使用前需要将下面的宏函数补充完整
#define spiex_write_pin(pin, level) gpio_set_level(pin, level)
#define spiex_read_pin(pin)         gpio_get_level(pin)

typedef struct
{
    uint32_t CS;
    uint32_t MISO;
    uint32_t MOSI;
    uint32_t SCK;
} spiex_t;

// 上层封装函数
void spiex_init(spiex_t* spi, uint32_t CS, uint32_t SCK, uint32_t MISO, uint32_t MOSI);
void spiex_write_cmd(spiex_t* spi, uint8_t cmd);
void spiex_write_data(spiex_t* spi, uint8_t* data, uint32_t size);
void spiex_read_data(spiex_t* spi, uint8_t* data, uint32_t size);

// 底层时序函数
void spiex_start(spiex_t* spi);
void spiex_stop(spiex_t* spi);
void spiex_swap_data(spiex_t* spi, uint8_t* send_data, uint8_t* recv_data, uint32_t size);
void spiex_send_data(spiex_t* spi, uint8_t* data, uint32_t size);
void spiex_receive_data(spiex_t* spi, uint8_t* data, uint32_t size);

#ifdef __cplusplus
}
#endif