#pragma once

#include "exspi.h"
#include "lvgl.h"
#include "lvgl_port.h"

#define TLCD    0
#if !(TLCD)
// ESP32S3 Core
#define CS          10
#define MOSI        11
#define SCLK        12
#define MISO        13
#define RES         14
#define DC          21
#define BLK         9
#else
// touch LCD
#define CS          7
#define MOSI        11
#define SCLK        12
#define MISO        13
#define RES         14
#define DC          21
#define BLK         9
#endif
// 正点原子
// #define CS          39
// #define MOSI        11
// #define SCLK        12
// #define MISO        13
// #define RES         38
// #define DC          40
// #define BLK         41

#if !(TLCD)
#define HOR_RES     160
#define VER_RES     80
#define OFFSET_X    1
#define OFFSET_Y    26
#define SPI_SPEED   60 // max surport speed:62.5MHz
#define LCD_PAGE    2
#define BLK_LEVEL   1
#define LCD_INDEV_ENCODER
#define LCD_ST7735
#else
#define SCL         6
#define SDA         5
#define HOR_RES     240
#define VER_RES     280
#define OFFSET_X    0
#define OFFSET_Y    20
#define SPI_SPEED   60 // max surport speed:62.5MHz
#define LCD_PAGE    7
#define BLK_LEVEL   1
#define ST7789_COLOR_INVERT
#define LCD_INDEV_TOUCH
#define LCD_ST7789
#endif
// #define LCD_ILI9341
// #define LCD_ST7789
// #define LCD_ST7735
#define LCD_LVGL
// #define LCD_LVGL_DEMO_WIDGETS
// #define LCD_LVGL_DEMO_STRESS
// #define LCD_LVGL_DEMO_BENCHMARK
// #define LCD_LVGL_DEMO_MUSIC

extern spi_device_handle_t lcd_spi;

void lcd_init(void);
void lcd_set_encoder_cnt(int encoder_count);
void lcd_set_encoder_key(int key_pressed);