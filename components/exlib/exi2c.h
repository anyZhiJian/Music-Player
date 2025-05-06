#pragma once

#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

i2c_master_bus_handle_t exi2c_master_init(i2c_port_num_t i2c_num, gpio_num_t scl, gpio_num_t sda);
void exi2c_master_deinit(i2c_master_bus_handle_t master_bus);
i2c_master_dev_handle_t exi2c_master_add_device(i2c_master_bus_handle_t master_bus, uint16_t device_address, uint32_t scl_speed_hz);
void exi2c_master_remove_device(i2c_master_dev_handle_t master_device);

#ifdef __cplusplus
}
#endif