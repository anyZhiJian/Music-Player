#pragma once

#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MOUNT_POINT "/sdcard"

esp_err_t exsd_card_sdmmc_init(gpio_num_t clk, gpio_num_t cmd, gpio_num_t d0, gpio_num_t d1, gpio_num_t d2, gpio_num_t d3);
esp_err_t exsd_card_sdspi_init(spi_host_device_t host_id, int cs);

#ifdef __cplusplus
}
#endif