#include "spiex.h"

void spiex_init(spiex_t* spi, uint32_t CS, uint32_t SCK, uint32_t MISO, uint32_t MOSI)
{
    spiex_write_pin(spi->CS, 1);
    spiex_write_pin(spi->SCK, 0);
    spi->CS = CS;
    spi->MISO = MISO;
    spi->MOSI = MOSI;
    spi->SCK = SCK;
}

void spiex_start(spiex_t* spi)
{
    spiex_write_pin(spi->CS, 0);
}

void spiex_stop(spiex_t* spi)
{
    spiex_write_pin(spi->CS, 1);
}

void spiex_swap_data(spiex_t* spi, uint8_t* send_data, uint8_t* recv_data, uint32_t size)
{
    uint32_t i, j;
    for(j = 0; j < size; j++)
    {
        *(recv_data + j) = 0x00;
        for (i = 0; i < 8; i++)
        {
            spiex_write_pin(spi->MOSI, *(send_data + j) & (0x80>>i));
            spiex_write_pin(spi->SCK, 1);
            if (spiex_read_pin(spi->MISO))
            {
                *(recv_data + j) |= (0x80>>i);
            }
            spiex_write_pin(spi->SCK, 0);
        }
    }
}

void spiex_send_data(spiex_t* spi, uint8_t* data, uint32_t size)
{
    uint32_t i, j;
    for(j = 0; j < size; j++)
    {
        for (i = 0; i < 8; i++)
        {
            spiex_write_pin(spi->MOSI, *(data + j) & (0x80>>i));
            spiex_write_pin(spi->SCK, 1);
            spiex_write_pin(spi->SCK, 0);
        }
    }
}

void spiex_receive_data(spiex_t* spi, uint8_t* data, uint32_t size)
{
    uint32_t i, j;
    for(j = 0; j < size; j++)
    {
        *(data + j) = 0x00;
        for (i = 0; i < 8; i++)
        {
            spiex_write_pin(spi->MOSI, 0);
            spiex_write_pin(spi->SCK, 1);
            if (spiex_read_pin(spi->MISO))
            {
                *(data + j) |= (0x80>>i);
            }
            spiex_write_pin(spi->SCK, 0);
        }
    }
}

void spiex_write_cmd(spiex_t* spi, uint8_t cmd)
{
    spiex_start(spi);
    spiex_send_data(spi, &cmd, 1);
    spiex_stop(spi);
}

void spiex_write_data(spiex_t* spi, uint8_t* data, uint32_t size)
{
    spiex_start(spi);
    spiex_send_data(spi, data, size);
    spiex_stop(spi);
}

void spiex_read_data(spiex_t* spi, uint8_t* data, uint32_t size)
{
    spiex_start(spi);
    spiex_receive_data(spi, data, size);
    spiex_stop(spi);
}
