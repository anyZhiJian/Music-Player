#include "lvgl_port.h"
#include "esp_heap_caps.h"

// 1 初始化和注册显示驱动
// 2 初始化和注册触摸驱动
// 3 初始化硬件接口
// 4 提供时钟给lvgl

// lv_disp_flush_ready();   需要在刷新准备好时调用
// lv_tick_inc();

static void lv_disp_init(lv_port_t* lv_port, lv_coord_t hor_res, lv_coord_t ver_res, uint8_t page, lv_port_disp_flush_cb_t flush_cb)
{
    int buf_size = hor_res * (ver_res / page);
    lv_color_t* buf1 = heap_caps_malloc(buf_size * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    lv_color_t* buf2 = heap_caps_malloc(buf_size * sizeof(lv_color_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    if(!buf1 || !buf2)
        return;
    lv_disp_draw_buf_init(&lv_port->draw_buf, buf1, buf2, buf_size);
    lv_disp_drv_init(&lv_port->disp_driver);
    lv_port->disp_driver.hor_res = hor_res;
    lv_port->disp_driver.ver_res = ver_res;
    lv_port->disp_driver.draw_buf = &lv_port->draw_buf;
    lv_port->disp_driver.flush_cb = flush_cb;
    lv_disp_drv_register(&lv_port->disp_driver);
}

static void lv_indev_init(lv_port_t* lv_port, lv_indev_type_t indev_type, lv_port_indev_read_cb_t read_cb)
{
    lv_indev_drv_init(&lv_port->indev_driver);
    lv_port->indev_driver.type = indev_type;
    lv_port->indev_driver.read_cb = read_cb;
    lv_indev_drv_register(&lv_port->indev_driver);
}

void lv_port_init(lv_port_t* lv_port, lv_coord_t hor_res, lv_coord_t ver_res, lv_indev_type_t indev_type, lv_port_disp_flush_cb_t flush_cb, lv_port_indev_read_cb_t read_cb, uint8_t page)
{
    lv_init();
    lv_disp_init(lv_port, hor_res, ver_res, page, flush_cb);
    lv_indev_init(lv_port, indev_type, read_cb);
}