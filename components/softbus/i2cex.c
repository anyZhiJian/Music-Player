#include "i2cex.h"

void i2cex_init(i2cex_t* i2c, uint32_t SCL, uint32_t SDA, uint32_t delay_us)
{
    i2cex_write_pin(i2c->SCL, 1);
    i2cex_write_pin(i2c->SDA, 1);
    i2c->SCL = SCL;
    i2c->SDA = SDA;
    i2c->delay_us = delay_us;
}

void i2cex_send_ack(i2cex_t* i2c, uint32_t ack)
{
    i2cex_write_pin(i2c->SDA, ack);
    i2cex_write_pin(i2c->SCL, 1);
    i2cex_delay(i2c->delay_us);
    i2cex_write_pin(i2c->SCL, 0);
}

uint32_t i2cex_receive_ack(i2cex_t* i2c)
{
    uint32_t ack;
    i2cex_write_pin(i2c->SDA, 1);
    i2cex_write_pin(i2c->SCL, 1);
    i2cex_delay(i2c->delay_us);
    ack = i2cex_read_pin(i2c->SDA);
    i2cex_write_pin(i2c->SCL, 0);
    return ack;
}

void i2cex_start(i2cex_t* i2c)
{
    i2cex_write_pin(i2c->SDA, 1);
    i2cex_write_pin(i2c->SCL, 1);
    i2cex_delay(i2c->delay_us);
    i2cex_write_pin(i2c->SDA, 0);
    i2cex_write_pin(i2c->SCL, 0);
}

void i2cex_stop(i2cex_t* i2c)
{
    i2cex_write_pin(i2c->SDA, 0);
    i2cex_write_pin(i2c->SCL, 1);
    i2cex_write_pin(i2c->SDA, 1);
}

uint32_t i2cex_send_addr(i2cex_t* i2c, uint8_t addr, i2cex_dir_t dir)
{
    uint32_t i;
    if(dir == i2cex_dir_receiver)
        addr |= 0x01;
    for(i = 0; i < 8; i++)
    {
        i2cex_write_pin(i2c->SDA, addr & (0x80>>i));
        i2cex_write_pin(i2c->SCL, 1);
        i2cex_delay(i2c->delay_us);
        i2cex_write_pin(i2c->SCL, 0);
    }
    return i2cex_receive_ack(i2c);
}

uint32_t i2cex_send_data(i2cex_t* i2c, uint8_t* data, uint32_t size)
{
    uint32_t ack = 0;
    uint32_t i, j;
    for(j = 0; j < size; j++)
    {
        for(i = 0; i < 8; i++)
        {
            i2cex_write_pin(i2c->SDA, data[j] & (0x80>>i));
            i2cex_write_pin(i2c->SCL, 1);
            i2cex_delay(i2c->delay_us);
            i2cex_write_pin(i2c->SCL, 0);
        }
        ack |= i2cex_receive_ack(i2c);
    }
    return ack;
}

void i2cex_receive_data(i2cex_t* i2c, uint8_t* data, uint32_t size)
{
    uint8_t recv_data;
    uint32_t i, j;
    for(j = 0; j < size; j++)
    {
        recv_data = 0x00;
        i2cex_write_pin(i2c->SDA, 1);
        for(i = 0; i < 8; i++)
        {
            i2cex_write_pin(i2c->SCL, 1);
            i2cex_delay(i2c->delay_us);
            if(i2cex_read_pin(i2c->SDA))
            {
                recv_data |= (0x80>>i);
            }
            i2cex_write_pin(i2c->SCL, 0);
        }
        if(j + 1 != size)
        {
            i2cex_send_ack(i2c, I2CEX_ACK);
        }
        data[j] = recv_data;
    }
    i2cex_send_ack(i2c, I2CEX_NACK);
}

uint32_t i2cex_write_cmd(i2cex_t* i2c, uint8_t addr, uint8_t reg, uint8_t cmd)
{
    uint32_t ack = I2CEX_ACK;
    i2cex_start(i2c);
    ack |= i2cex_send_addr(i2c, addr, i2cex_dir_transmitter);
    ack |= i2cex_send_data(i2c, &reg, 1);
    ack |= i2cex_send_data(i2c, &cmd, 1);
    i2cex_stop(i2c);
    return ack;
}

uint32_t i2cex_write_data(i2cex_t* i2c, uint8_t addr, uint8_t cmd, uint8_t* data, uint32_t size)
{
    uint32_t ack = I2CEX_ACK;
    i2cex_start(i2c);
    ack |= i2cex_send_addr(i2c, addr, i2cex_dir_transmitter);
    ack |= i2cex_send_data(i2c, &cmd, 1);
    ack |= i2cex_send_data(i2c, data, size);
    i2cex_stop(i2c);
    return ack;
}

uint32_t i2cex_read_data(i2cex_t* i2c, uint8_t addr, uint8_t cmd, uint8_t* data, uint32_t size)
{
    uint32_t ack = I2CEX_ACK;
    i2cex_start(i2c);
    ack |= i2cex_send_addr(i2c, addr, i2cex_dir_transmitter);
    ack |= i2cex_send_data(i2c, &cmd, 1);
    i2cex_start(i2c);
    ack |= i2cex_send_addr(i2c, addr, i2cex_dir_receiver);
    i2cex_receive_data(i2c, data, size);
    i2cex_stop(i2c);
    return ack;
}
