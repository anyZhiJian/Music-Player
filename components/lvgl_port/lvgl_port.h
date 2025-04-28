#pragma once

#include "lvgl.h"

typedef void (*lv_port_disp_flush_cb_t)(struct _lv_disp_drv_t*, const lv_area_t*, lv_color_t*);
typedef void (*lv_port_indev_read_cb_t)(struct _lv_indev_drv_t*, lv_indev_data_t*);

typedef struct
{
    lv_disp_drv_t disp_driver;
    lv_disp_draw_buf_t draw_buf;
    lv_indev_drv_t indev_driver;
} lv_port_t;

void lv_port_init(lv_port_t* lv_port, lv_coord_t hor_res, lv_coord_t ver_res, lv_indev_type_t indev_type, lv_port_disp_flush_cb_t flush_cb, lv_port_indev_read_cb_t read_cb, uint8_t page);
