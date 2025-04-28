#pragma once

#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

// 使用前需要将下面的宏函数补充完整
#define i2cex_write_pin(pin, level) gpio_set_level(pin, level)
#define i2cex_read_pin(pin)         gpio_get_level(pin)
#define i2cex_delay(us)             {uint32_t tick = (us);while(tick--);}

#define I2CEX_ACK   0
#define I2CEX_NACK  1

typedef struct
{
    uint32_t delay_us;
    uint32_t SCL;
    uint32_t SDA;
} i2cex_t;

typedef enum
{
    i2cex_dir_transmitter,
    i2cex_dir_receiver,
} i2cex_dir_t;

// 上层封装函数
void i2cex_init(i2cex_t* i2c, uint32_t SCL, uint32_t SDA, uint32_t delay_us);
uint32_t i2cex_write_cmd(i2cex_t* i2c, uint8_t addr, uint8_t reg, uint8_t cmd);
uint32_t i2cex_write_data(i2cex_t* i2c, uint8_t addr, uint8_t cmd, uint8_t* data, uint32_t size);
uint32_t i2cex_read_data(i2cex_t* i2c, uint8_t addr, uint8_t cmd, uint8_t* data, uint32_t size);

// 底层时序函数
void i2cex_start(i2cex_t* i2c);
void i2cex_stop(i2cex_t* i2c);
void i2cex_send_ack(i2cex_t* i2c, uint32_t ack);
uint32_t i2cex_receive_ack(i2cex_t* i2c);
uint32_t i2cex_send_addr(i2cex_t* i2c, uint8_t addr, i2cex_dir_t dir);
uint32_t i2cex_send_data(i2cex_t* i2c, uint8_t* data, uint32_t size);
void i2cex_receive_data(i2cex_t* i2c, uint8_t* data, uint32_t size);

#ifdef __cplusplus
}
#endif