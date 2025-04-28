#include "exi2c.h"

i2c_master_bus_handle_t exi2c_init(i2c_port_num_t i2c_num, gpio_num_t scl, gpio_num_t sda)
{
    i2c_master_bus_config_t i2c_mst_config = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = i2c_num,
    .scl_io_num = scl,
    .sda_io_num = sda,
    .glitch_ignore_cnt = 7,
    .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus_handle = NULL;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));
    return bus_handle;
}

i2c_master_dev_handle_t exi2c_add_device(i2c_master_bus_handle_t bus_handle, uint16_t device_address, uint32_t scl_speed_hz)
{
    i2c_device_config_t dev_cfg = {
    .dev_addr_length = I2C_ADDR_BIT_LEN_7,
    .device_address = device_address,
    .scl_speed_hz = scl_speed_hz,
    };
    i2c_master_dev_handle_t dev_handle = NULL;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));
    return dev_handle;
}