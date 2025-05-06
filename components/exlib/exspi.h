#pragma once

#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXSPI_QUEUE_SIZE    10

void exspi_init(spi_host_device_t host_id, int sclk, int mosi, int miso, int max_transfer_sz);
void exspi_deinit(spi_host_device_t host_id);
spi_device_handle_t exspi_add_device(spi_host_device_t host_id, int clock_speed_hz, int cs, transaction_cb_t pre_cb, transaction_cb_t post_cb);
void exspi_remove_device(spi_device_handle_t device);
void exspi_cmd(spi_device_handle_t device, const uint8_t cmd, bool keep_cs_active);
void exspi_data(spi_device_handle_t device, const void *data, size_t length);
void exspi_byte(spi_device_handle_t device, const uint8_t data);

#ifdef __cplusplus
}
#endif