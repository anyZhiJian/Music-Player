#include "exspi.h"
#include "driver/gpio.h"
#include <string.h>

void exspi_init(spi_host_device_t host_id, int sclk, int mosi, int miso, int max_transfer_sz)
{
    spi_bus_config_t bus_cfg = {
        .miso_io_num = miso,
        .mosi_io_num = mosi,
        .sclk_io_num = sclk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = max_transfer_sz,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(host_id, &bus_cfg, SPI_DMA_CH_AUTO));
}

void exspi_deinit(spi_host_device_t host_id)
{
    ESP_ERROR_CHECK(spi_bus_free(host_id));
}

spi_device_handle_t exspi_add_device(spi_host_device_t host_id, int clock_speed_hz, int cs, transaction_cb_t pre_cb, transaction_cb_t post_cb)
{
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = clock_speed_hz,
        .mode = 0,
        .spics_io_num = cs,
        .queue_size = EXSPI_QUEUE_SIZE,
        .pre_cb = pre_cb,
        .post_cb = post_cb,
    };
    spi_device_handle_t device = NULL;
    ESP_ERROR_CHECK(spi_bus_add_device(host_id, &dev_cfg, &device));
    return device;
}

void exspi_remove_device(spi_device_handle_t device)
{
    ESP_ERROR_CHECK(spi_bus_remove_device(device));
}

void exspi_cmd(spi_device_handle_t device, const uint8_t cmd, bool keep_cs_active)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*)0; // set DC
    if (keep_cs_active)
        t.flags = SPI_TRANS_CS_KEEP_ACTIVE;
    ret = spi_device_polling_transmit(device, &t);
    assert(ret == ESP_OK);
}

void exspi_data(spi_device_handle_t device, const void *data, size_t length)
{
    esp_err_t ret;
    spi_transaction_t t;
    if (length == 0)
        return;
    memset(&t, 0, sizeof(t));
    t.length = length * 8;
    t.tx_buffer = data;
    t.user = (void*)1; // set DC
    ret = spi_device_polling_transmit(device, &t);
    assert(ret == ESP_OK);
}

void exspi_byte(spi_device_handle_t device, const uint8_t data)
{
    exspi_data(device, &data, 1);
}