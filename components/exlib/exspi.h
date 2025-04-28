#pragma once

#include "driver/spi_master.h"

#ifdef __cplusplus
extern "C" {
#endif

void exspi_init(spi_host_device_t host_id, int sclk, int mosi, int miso, int max_transfer_sz);
void exspi_add_device(spi_host_device_t host_id, spi_device_handle_t *spi, int clock_speed_hz, int cs, transaction_cb_t pre_cb, transaction_cb_t post_cb);
void exspi_cmd(spi_device_handle_t spi, const uint8_t cmd, bool keep_cs_active);
void exspi_data(spi_device_handle_t spi, const uint8_t *data, int len);
void exspi_byte(spi_device_handle_t spi, const uint8_t data);

#ifdef __cplusplus
}
#endif