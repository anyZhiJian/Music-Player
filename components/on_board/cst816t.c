#include "cst816t.h"

#define CST816T_ADDR    0x15

static uint8_t reg_touch_cnt = 0x02;
static uint8_t reg_x = 0x03;
static uint8_t reg_y = 0x05;

cst816t_handle_t cst816t_init(i2c_port_num_t i2c_num, gpio_num_t scl, gpio_num_t sda, uint16_t limit_x, uint16_t limit_y)
{
    cst816t_t* cst816t = malloc(sizeof(cst816t_t));
    assert(cst816t);
    cst816t->i2c = exi2c_init(i2c_num, scl, sda);
    cst816t->dev = exi2c_add_device(cst816t->i2c, CST816T_ADDR, 400000);
    cst816t->limit_x = limit_x;
    cst816t->limit_y = limit_y;
    return cst816t;
}

void cst816t_read(cst816t_handle_t cst816t, int16_t* x, int16_t* y, int* touch_state)
{
    assert(x);
    assert(y);
    assert(touch_state);
    uint8_t touch_cnt = 0;
    static int16_t last_x = 0;  // 12bit pixel value
    static int16_t last_y = 0;  // 12bit pixel value
    i2c_master_transmit_receive(cst816t->dev, &reg_touch_cnt, 1, &touch_cnt, 1, 1000);
    if (touch_cnt != 1)
    {
        *x = last_x;
        *y = last_y;
        *touch_state = 0;
        return;
    }
    uint8_t read_x[2];
    uint8_t read_y[2];
    i2c_master_transmit_receive(cst816t->dev, &reg_x, 1, read_x, 2, 1000);
    i2c_master_transmit_receive(cst816t->dev, &reg_y, 1, read_y, 2, 1000);
    *x = ((read_x[0] & 0x0F) << 8) | (read_x[1] & 0xFF);
    *y = ((read_y[0] & 0x0F) << 8) | (read_y[1] & 0xFF);
    if(*x >= cst816t->limit_x)
        *x = cst816t->limit_x - 1;
    if(*y >= cst816t->limit_y)
        *y = cst816t->limit_y - 1;
    last_x = *x;
    last_y = *y;
    *touch_state = 1;
}