#pragma once

#include "exi2c.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    i2c_master_bus_handle_t i2c;
    i2c_master_dev_handle_t dev;
    uint16_t limit_x;
    uint16_t limit_y;
} cst816t_t;

typedef cst816t_t* cst816t_handle_t;

cst816t_handle_t cst816t_init(i2c_port_num_t i2c_num, gpio_num_t scl, gpio_num_t sda, uint16_t limit_x, uint16_t limit_y);
void cst816t_read(cst816t_handle_t cst816t, int16_t* x, int16_t* y, int* touch_state);

#ifdef __cplusplus
}
#endif