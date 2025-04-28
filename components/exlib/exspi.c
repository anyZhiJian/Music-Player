#include "exspi.h"
#include "driver/gpio.h"
#include <string.h>

void exspi_init(spi_host_device_t host_id, int sclk, int mosi, int miso, int max_transfer_sz)
{
    spi_bus_config_t buscfg = {
        .miso_io_num = miso,
        .mosi_io_num = mosi,
        .sclk_io_num = sclk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = max_transfer_sz,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(host_id, &buscfg, SPI_DMA_CH_AUTO));
}

void exspi_add_device(spi_host_device_t host_id, spi_device_handle_t *spi, int clock_speed_hz, int cs, transaction_cb_t pre_cb, transaction_cb_t post_cb)
{
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = clock_speed_hz,
        .mode = 0,
        .spics_io_num = cs,
        .queue_size = 10,
        .pre_cb = pre_cb,
        .post_cb = post_cb,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(host_id, &devcfg, spi));
}

void exspi_cmd(spi_device_handle_t spi, const uint8_t cmd, bool keep_cs_active)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length = 8;                   //Command is 8 bits
    t.tx_buffer = &cmd;             //The data is the cmd itself
    t.user = (void*)0;              //D/C needs to be set to 0
    if (keep_cs_active) {
        t.flags = SPI_TRANS_CS_KEEP_ACTIVE;   //Keep CS active after data transfer
    }
    ret = spi_device_polling_transmit(spi, &t); //Transmit!
    assert(ret == ESP_OK);          //Should have had no issues.
}

void exspi_data(spi_device_handle_t spi, const uint8_t *data, int len)
{
    esp_err_t ret;
    spi_transaction_t t;
    if (len == 0) {
        return;    //no need to send anything
    }
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length = len * 8;             //Len is in bytes, transaction length is in bits.
    t.tx_buffer = data;             //Data
    t.user = (void*)1;              //D/C needs to be set to 1
    ret = spi_device_polling_transmit(spi, &t); //Transmit!
    assert(ret == ESP_OK);          //Should have had no issues.
}

void exspi_byte(spi_device_handle_t spi, const uint8_t data)
{
    exspi_data(spi, &data, 1);
}