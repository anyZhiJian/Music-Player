#pragma once

#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MOUNT_POINT "/sdcard"

esp_err_t exsdmmc_init(gpio_num_t clk, gpio_num_t cmd, gpio_num_t d0, gpio_num_t d1, gpio_num_t d2, gpio_num_t d3);
esp_err_t exsdspi_init(int sclk, int mosi, int miso, int cs);

#ifdef __cplusplus
}
#endif