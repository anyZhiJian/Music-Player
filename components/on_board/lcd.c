#include "lcd.h"
#include "exgpio.h"
#include "exgptimer.h"
#include "cst816t.h"
#include "lvgl.h"
#include "lvgl_port.h"
#include "lv_demos.h"

spi_device_handle_t lcd_spi;
cst816t_handle_t cst816t;
static lv_port_t lcd_lv_port;
static gptimer_handle_t lcd_gptimer;

void lcd_gpio_init(void);
void lcd_set_region(spi_device_handle_t spi, int x1, int y1, int x2, int y2);
void lcd_ili0341_init(spi_device_handle_t spi);
void lcd_st7789_init(spi_device_handle_t spi);
void lcd_st7735_init(spi_device_handle_t spi);
void lcd_pre_cb(spi_transaction_t *t);
bool lcd_timer_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx);
void lcd_flush_cb(struct _lv_disp_drv_t* driver, const lv_area_t* area, lv_color_t* data);
void lcd_read_cb(struct _lv_indev_drv_t* driver, lv_indev_data_t* data);

void lcd_init(void)
{
    exspi_init(SPI2_HOST, SCLK, MOSI, MISO, HOR_RES * 120 * 2);
    lcd_spi = exspi_add_device(SPI2_HOST, SPI_SPEED * 1000 * 1000, CS, lcd_pre_cb, NULL);
    lcd_gptimer = exgptimer_init(100000, 0, 1000, true);
    exgptimer_register_cbs(lcd_gptimer, lcd_timer_cb, NULL);
    exgptimer_enable_and_start(lcd_gptimer);

    lcd_gpio_init();
    gpio_set_level(RES, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(RES, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

#ifdef LCD_ILI9341
    lcd_ili0341_init(lcd_spi);
#endif
#ifdef LCD_ST7789
    lcd_st7789_init(lcd_spi);
#endif
#ifdef LCD_ST7735
    lcd_st7735_init(lcd_spi);
#endif

    lcd_set_region(lcd_spi, 0, 0, HOR_RES-1, VER_RES-1);
    gpio_set_level(BLK, BLK_LEVEL);

    lv_port_indev_read_cb_t read_cb = NULL;
#ifdef LCD_INDEV
    cst816t = cst816t_init(I2C_NUM_0, SCL, SDA, HOR_RES, VER_RES);
    read_cb = lcd_read_cb;
#endif
#ifdef LCD_LVGL
    lv_port_init(&lcd_lv_port, HOR_RES, VER_RES, LV_INDEV_TYPE_POINTER, lcd_flush_cb, read_cb, LCD_PAGE);
#ifdef LCD_LVGL_DEMO_WIDGETS
    lv_demo_widgets();
#endif
#ifdef LCD_LVGL_DEMO_STRESS
    lv_demo_stress();
#endif
#ifdef LCD_LVGL_DEMO_BENCHMARK
    lv_demo_benchmark();
#endif
#ifdef LCD_LVGL_DEMO_MUSIC
    lv_demo_music();
#endif
#endif
}

void lcd_gpio_init(void)
{
    exgpio_init(EXGPIO_PIN_MASK(DC), GPIO_MODE_OUTPUT, 1, 0, 0);
    exgpio_init(EXGPIO_PIN_MASK(RES), GPIO_MODE_OUTPUT, 1, 0, 0);
    exgpio_init(EXGPIO_PIN_MASK(BLK), GPIO_MODE_OUTPUT, 1, 0, 0);
}

void lcd_set_region(spi_device_handle_t spi, int x1, int y1, int x2, int y2)
{
    exspi_cmd(spi, 0x2A, false);
    exspi_byte(spi, x1>>8);
    exspi_byte(spi, x1);
    exspi_byte(spi, x2>>8);
    exspi_byte(spi, x2);
    exspi_cmd(spi, 0x2B, false);
    exspi_byte(spi, y1>>8);
    exspi_byte(spi, y1);
    exspi_byte(spi, y2>>8);
    exspi_byte(spi, y2);
    exspi_cmd(spi, 0x2C, false);
}

void lcd_ili0341_init(spi_device_handle_t spi)
{
    exspi_cmd(spi, 0x11, false);
    vTaskDelay(pdMS_TO_TICKS(120));

    exspi_cmd(spi, 0xCF, false);
    exspi_byte(spi, 0x00);
    exspi_byte(spi, 0xC1);
    exspi_byte(spi, 0X30);

    exspi_cmd(spi, 0xED, false);
    exspi_byte(spi, 0x64);
    exspi_byte(spi, 0x03);
    exspi_byte(spi, 0X12);
    exspi_byte(spi, 0X81);

    exspi_cmd(spi, 0xE8, false);
    exspi_byte(spi, 0x85);
    exspi_byte(spi, 0x00);
    exspi_byte(spi, 0x79);

    exspi_cmd(spi, 0xCB, false);
    exspi_byte(spi, 0x39);
    exspi_byte(spi, 0x2C);
    exspi_byte(spi, 0x00);
    exspi_byte(spi, 0x34);
    exspi_byte(spi, 0x02);

    exspi_cmd(spi, 0xF7, false);
    exspi_byte(spi, 0x20);

    exspi_cmd(spi, 0xEA, false);
    exspi_byte(spi, 0x00);
    exspi_byte(spi, 0x00);

    exspi_cmd(spi, 0xC0, false);
    exspi_byte(spi, 0x1D);

    exspi_cmd(spi, 0xC1, false);
    exspi_byte(spi, 0x12);

    exspi_cmd(spi, 0xC5, false);
    exspi_byte(spi, 0x33);
    exspi_byte(spi, 0x3F);

    exspi_cmd(spi, 0xC7, false);
    exspi_byte(spi, 0x92);

    exspi_cmd(spi, 0x3A, false);
    exspi_byte(spi, 0x55);

    exspi_cmd(spi, 0x36, false);
    exspi_byte(spi, 0x08);
//    exspi_byte(spi, 0xC8);
//    exspi_byte(spi, 0x78);
//    exspi_byte(spi, 0xA8);

    exspi_cmd(spi, 0xB1, false);
    exspi_byte(spi, 0x00);
    exspi_byte(spi, 0x12);

    exspi_cmd(spi, 0xB6, false);
    exspi_byte(spi, 0x0A);
    exspi_byte(spi, 0xA2);

    exspi_cmd(spi, 0x44, false);
    exspi_byte(spi, 0x02);

    exspi_cmd(spi, 0xF2, false);
    exspi_byte(spi, 0x00);

    exspi_cmd(spi, 0x26, false);
    exspi_byte(spi, 0x01);

    exspi_cmd(spi, 0xE0, false);
    exspi_byte(spi, 0x0F);
    exspi_byte(spi, 0x22);
    exspi_byte(spi, 0x1C);
    exspi_byte(spi, 0x1B);
    exspi_byte(spi, 0x08);
    exspi_byte(spi, 0x0F);
    exspi_byte(spi, 0x48);
    exspi_byte(spi, 0xB8);
    exspi_byte(spi, 0x34);
    exspi_byte(spi, 0x05);
    exspi_byte(spi, 0x0C);
    exspi_byte(spi, 0x09);
    exspi_byte(spi, 0x0F);
    exspi_byte(spi, 0x07);
    exspi_byte(spi, 0x00);

    exspi_cmd(spi, 0XE1, false);
    exspi_byte(spi, 0x00);
    exspi_byte(spi, 0x23);
    exspi_byte(spi, 0x24);
    exspi_byte(spi, 0x07);
    exspi_byte(spi, 0x10);
    exspi_byte(spi, 0x07);
    exspi_byte(spi, 0x38);
    exspi_byte(spi, 0x47);
    exspi_byte(spi, 0x4B);
    exspi_byte(spi, 0x0A);
    exspi_byte(spi, 0x13);
    exspi_byte(spi, 0x06);
    exspi_byte(spi, 0x30);
    exspi_byte(spi, 0x38);
    exspi_byte(spi, 0x0F);
    
    exspi_cmd(spi, 0x29, false);
}

void lcd_st7789_init(spi_device_handle_t spi)
{
    exspi_cmd(spi, 0x11, false);
    vTaskDelay(pdMS_TO_TICKS(120));
    exspi_cmd(spi, 0X36, false);
    exspi_byte(spi, 0x00);
    // else if(USE_HORIZONTAL==1)exspi_byte(spi, 0xC0);
    // else if(USE_HORIZONTAL==2)exspi_byte(spi, 0x70);
    // else exspi_byte(spi, 0xA0);
    exspi_cmd(spi, 0X3A, false);
    exspi_byte(spi, 0X05);

    exspi_cmd(spi, 0xb2, false);
    exspi_byte(spi, 0x0c);
    exspi_byte(spi, 0x0c);
    exspi_byte(spi, 0x00);
    exspi_byte(spi, 0x33);
    exspi_byte(spi, 0x33);
    exspi_cmd(spi, 0xb7, false);
    exspi_byte(spi, 0x35);

    exspi_cmd(spi, 0xbb, false);
    exspi_byte(spi, 0x35);
    exspi_cmd(spi, 0xc0, false);
    exspi_byte(spi, 0x2c);
    exspi_cmd(spi, 0xc2, false);
    exspi_byte(spi, 0x01);
    exspi_cmd(spi, 0xc3, false);
    exspi_byte(spi, 0x13);
    exspi_cmd(spi, 0xc4, false);
    exspi_byte(spi, 0x20);
    exspi_cmd(spi, 0xc6, false);
    exspi_byte(spi, 0x0f);
    exspi_cmd(spi, 0xca, false);
    exspi_byte(spi, 0x0f);
    exspi_cmd(spi, 0xc8, false);
    exspi_byte(spi, 0x08);
    exspi_cmd(spi, 0x55, false);
    exspi_byte(spi, 0x90);
    exspi_cmd(spi, 0xd0, false);
    exspi_byte(spi, 0xa4);
    exspi_byte(spi, 0xa1);

    exspi_cmd(spi, 0xe0, false);
    exspi_byte(spi, 0xd0);
    exspi_byte(spi, 0x00);
    exspi_byte(spi, 0x06);
    exspi_byte(spi, 0x09);
    exspi_byte(spi, 0x0b);
    exspi_byte(spi, 0x2a);
    exspi_byte(spi, 0x3c);
    exspi_byte(spi, 0x55);
    exspi_byte(spi, 0x4b);
    exspi_byte(spi, 0x08);
    exspi_byte(spi, 0x16);
    exspi_byte(spi, 0x14);
    exspi_byte(spi, 0x19);
    exspi_byte(spi, 0x20);
    exspi_cmd(spi, 0xe1, false);
    exspi_byte(spi, 0xd0);
    exspi_byte(spi, 0x00);
    exspi_byte(spi, 0x06);
    exspi_byte(spi, 0x09);
    exspi_byte(spi, 0x0b);
    exspi_byte(spi, 0x29);
    exspi_byte(spi, 0x36);
    exspi_byte(spi, 0x54);
    exspi_byte(spi, 0x4b);
    exspi_byte(spi, 0x0d);
    exspi_byte(spi, 0x16);
    exspi_byte(spi, 0x14);
    exspi_byte(spi, 0x21);
    exspi_byte(spi, 0x20);

#ifdef ST7789_COLOR_INVERT
    exspi_cmd(spi, 0x21, false);
#endif

    exspi_cmd(spi, 0x29, false);
}

void lcd_st7735_init(spi_device_handle_t spi)
{
    exspi_cmd(spi, 0x11, false);
    vTaskDelay(pdMS_TO_TICKS(120));
    exspi_cmd(spi, 0xB1, false);
    exspi_byte(spi, 0x05);
    exspi_byte(spi, 0x3C);
    exspi_byte(spi, 0x3C);
    exspi_cmd(spi, 0xB2, false);
    exspi_byte(spi, 0x05);
    exspi_byte(spi, 0x3C);
    exspi_byte(spi, 0x3C);
    exspi_cmd(spi, 0xB3, false);
    exspi_byte(spi, 0x05);
    exspi_byte(spi, 0x3C);
    exspi_byte(spi, 0x3C);
    exspi_byte(spi, 0x05);
    exspi_byte(spi, 0x3C);
    exspi_byte(spi, 0x3C);
    exspi_cmd(spi, 0xB4, false);
    exspi_byte(spi, 0x03);
    exspi_cmd(spi, 0xC0, false);
    exspi_byte(spi, 0xAB);
    exspi_byte(spi, 0x0B);
    exspi_byte(spi, 0x04);
    exspi_cmd(spi, 0xC1, false);
    exspi_byte(spi, 0xC5);
    exspi_cmd(spi, 0xC2, false);
    exspi_byte(spi, 0x0D);
    exspi_byte(spi, 0x00);
    exspi_cmd(spi, 0xC3, false);
    exspi_byte(spi, 0x8D);
    exspi_byte(spi, 0x6A);
    exspi_cmd(spi, 0xC4, false);
    exspi_byte(spi, 0x8D);
    exspi_byte(spi, 0xEE);
    exspi_cmd(spi, 0xC5, false);
    exspi_byte(spi, 0x0F);
    exspi_cmd(spi, 0xE0, false);
    exspi_byte(spi, 0x07);
    exspi_byte(spi, 0x0E);
    exspi_byte(spi, 0x08);
    exspi_byte(spi, 0x07);
    exspi_byte(spi, 0x10);
    exspi_byte(spi, 0x07);
    exspi_byte(spi, 0x02);
    exspi_byte(spi, 0x07);
    exspi_byte(spi, 0x09);
    exspi_byte(spi, 0x0F);
    exspi_byte(spi, 0x25);
    exspi_byte(spi, 0x36);
    exspi_byte(spi, 0x00);
    exspi_byte(spi, 0x08);
    exspi_byte(spi, 0x04);
    exspi_byte(spi, 0x10);
    exspi_cmd(spi, 0xE1, false);
    exspi_byte(spi, 0x0A);
    exspi_byte(spi, 0x0D);
    exspi_byte(spi, 0x08);
    exspi_byte(spi, 0x07);
    exspi_byte(spi, 0x0F);
    exspi_byte(spi, 0x07);
    exspi_byte(spi, 0x02);
    exspi_byte(spi, 0x07);
    exspi_byte(spi, 0x09);
    exspi_byte(spi, 0x0F);
    exspi_byte(spi, 0x25);
    exspi_byte(spi, 0x35);
    exspi_byte(spi, 0x00);
    exspi_byte(spi, 0x09);
    exspi_byte(spi, 0x04);
    exspi_byte(spi, 0x10);
    exspi_cmd(spi, 0xFC, false);
    exspi_byte(spi, 0x80);
    exspi_cmd(spi, 0x3A, false);
    exspi_byte(spi, 0x05);
    // direction
    exspi_cmd(spi, 0x36, false);
    // exspi_byte(spi, 0x08);
    // exspi_byte(spi, 0xC8);
    // exspi_byte(spi, 0x78);
    exspi_byte(spi, 0xA8);

    exspi_cmd(spi, 0x21, false);
    exspi_cmd(spi, 0x29, false);
    exspi_cmd(spi, 0x2A, false);
    exspi_byte(spi, 0x00);
    exspi_byte(spi, 0x1A);
    exspi_byte(spi, 0x00);
    exspi_byte(spi, 0x69);
    exspi_cmd(spi, 0x2B, false);
    exspi_byte(spi, 0x00);
    exspi_byte(spi, 0x01);
    exspi_byte(spi, 0x00);
    exspi_byte(spi, 0xA0);
    exspi_cmd(spi, 0x2C, false);
}

void lcd_pre_cb(spi_transaction_t *t)
{
    gpio_set_level(DC, (int)t->user);
}

bool lcd_timer_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    lv_tick_inc(10);
    return pdTRUE;
}

void lcd_flush_cb(struct _lv_disp_drv_t* driver, const lv_area_t* area, lv_color_t* data)
{
    lcd_set_region(lcd_spi, area->x1 + OFFSET_X, area->y1 + OFFSET_Y, area->x2 + OFFSET_X, area->y2 + OFFSET_Y);
    exspi_data(lcd_spi, (const uint8_t*)data, (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1) * 2);
    lv_disp_flush_ready(&lcd_lv_port.disp_driver);
}

void IRAM_ATTR lcd_read_cb(struct _lv_indev_drv_t* driver, lv_indev_data_t* data)
{
    cst816t_read(cst816t, &data->point.x, &data->point.y, (int*)&data->state);
}