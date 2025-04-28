#pragma once

#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

i2c_master_bus_handle_t exi2c_init(i2c_port_num_t i2c_num, gpio_num_t scl, gpio_num_t sda);
i2c_master_dev_handle_t exi2c_add_device(i2c_master_bus_handle_t bus_handle, uint16_t device_address, uint32_t scl_speed_hz);

#ifdef __cplusplus
}
#endif