/**
 * 图形库模块
 *
 * 注意事项：
 * 1 图形库适用于OLED和LCD
 * 2 在OLED和LCD上有不同的颜色模式：
 *     1 OLED：
 *         1 color_mono_light：     1亮0灭
 *         2 color_mono_dark：      0亮1灭
 *         3 color_mono_inverse：   1取反0不变
 *     2 LCD：RGB565模式
 * 3 默认字体为 font_Song_Typeface_8x16 字体
 * 4 支持垂直分页模式，可在内存偏少的情况下工作，但运行效率会显著下降
 * 5 支持自定义字体，详情查看 font.h
 *
 */

#ifndef __GRAPHICSLIB_H
#define __GRAPHICSLIB_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "font.h"

#define sgl_malloc malloc
#define sgl_free free
#define sgl_memset memset

#define sgl_FORMAT_STRING_BUFFERSIZE 100

#define color_mono_light 0
#define color_mono_dark 1
#define color_mono_inverse 2

typedef enum
{
    sgl_mode_full_buffer,
    sgl_mode_page,
    sgl_mode_Partial,
} sgl_mode_t;

typedef enum
{
    sgl_bs_1bit,
    sgl_bs_16bit,
} sgl_buffer_struct_t;

typedef enum
{
    sgl_fillstyle_filled,
    sgl_fillstyle_outlined,
} sgl_fillstyle_t;

typedef enum
{
    sgl_dir_up = 0,
    sgl_dir_down,
    sgl_dir_left,
    sgl_dir_right,
} sgl_dir_t;

#define sgl_dir_default sgl_dir_up

typedef enum
{
    sgl_align_up_left = 0,
    sgl_align_up_right,
    sgl_align_down_left,
    sgl_align_down_right,
    sgl_align_center,
    sgl_align_up_center,
    sgl_align_down_center,
    sgl_align_left_center,
    sgl_align_right_center,
} sgl_align_t;

#define sgl_align_default sgl_align_up_left

/**
 * alignment:
 *
 *     up_left           up_center           up_right
 *             +-------------+-------------+
 *             |                           |
 *             |                           |
 * left_center +        center(x,y)        + right_center
 *             |                           |
 *             |                           |
 *             +-------------+-------------+
 *   down_left          down_center          down_right
 *
 */

typedef enum
{
    sgl_rotate_0 = 0,
    sgl_rotate_90,
    sgl_rotate_180,
    sgl_rotate_270,
} sgl_rotate_t;

#define sgl_rotate_default sgl_rotate_0

/**
 * rotate shape:坐标轴固定，图像旋转
 *
 * -------------------------
 * |rotate |   x'  |   y'  |
 * |-------|-------|-------|
 * |   0   |   x   |   y   |
 * |   90  |  w-y  |   x   |
 * |  -90  |   y   |  h-x  |
 * |  180  |  w-x  |  h-y  |
 * -------------------------
 *
 * rotate axis:图像固定，坐标轴旋转
 *
 * -------------------------
 * |rotate |   x'  |   y'  |
 * |-------|-------|-------|
 * |   0   |   x   |   y   |
 * |   90  |   y   |  w-x  |
 * |  -90  |  h-y  |   x   |
 * |  180  |  w-x  |  h-y  |
 * -------------------------
 *
 * w(width):hor_res-1  h(height):ver_res-1
 *
 */

typedef enum
{
    sgl_circlemode_upper_right = 0x01,
    sgl_circlemode_upper_left = 0x02,
    sgl_circlemode_lower_right = 0x04,
    sgl_circlemode_lower_left = 0x08,
    circlemode_all = 0xFF,
} sgl_circlemode_t;

/**
 * buffer structrue:
 *
 * |------------width------------|
 * ----------buffer_width----------------start_y-------
 * |                             |     |              |
 * |           page_1            | buffer_height      |
 * |                             |     |              |
 * |-----------screen------------|--------end_y-----height
 * |                             |                    |
 * |           page_2            |                    |
 * |                             |                    |
 * ----------------------------------------------------
 *
 */

typedef struct
{
    int left;
    int right;
    uint32_t top;
    uint32_t bottom;
} sgl_rect_t, *sgl_rect_handle_t;

typedef struct sgl_painter_t
{
    uint8_t *buffer;                                  // 缓冲区指针
    int buffer_size;                                  // 缓冲区大小
    int hor_res;                                      // 屏幕水平分辨率(X坐标范围的大小)
    int ver_res;                                      // 屏幕垂直分辨率(Y坐标范围的大小)
    int page_start_row;                               // 页起始行
    int page_width;                                   // 页宽
    int offset_x;                                     // X坐标偏移
    int offset_y;                                     // Y坐标偏移
    int buffer_width;                                 // 缓冲区宽度
    int buffer_height;                                // 缓冲区高度
    sgl_rect_t mask;                                  // 可绘制的矩形区域
    fontinfo_t font_info;                             // 字体信息
    sgl_rotate_t rotate;                              // 屏幕旋转参数
    uint32_t background;                              // 背景颜色(默认为0)
    sgl_mode_t mode;                                  // 绘制模式
    void (*draw_piexl)(struct sgl_painter_t *painter, // 绘制像素函数指针
                       int x, int y, uint32_t color);
    void (*draw)(struct sgl_painter_t *painter);       // 绘制回调函数
    void (*flush)(void *buffer, uint32_t buffer_size); // 发送数据回调函数
    void (*frame_start_cb)(void);                      // 帧起始回调函数
    void (*frame_end_cb)(void);                        // 帧结束回调函数
} sgl_painter_t, *sgl_painter_handle_t;

void sgl_painter_init(sgl_painter_handle_t painter, void *buffer,
                      uint32_t buffer_size, uint32_t hor_res, uint32_t ver_res,
                      sgl_buffer_struct_t bs, sgl_mode_t mode);
void sgl_paint_start(sgl_painter_handle_t painter);
void sgl_paint_end(sgl_painter_handle_t painter);
void sgl_display(sgl_painter_handle_t painter);
void sgl_set_draw(sgl_painter_handle_t painter,
                  void (*draw)(sgl_painter_handle_t));
void sgl_set_flush(sgl_painter_handle_t painter,
                   void (*flush)(void *, uint32_t));
void sgl_set_draw_piexl(sgl_painter_handle_t painter,
                        void (*draw_piexl)(sgl_painter_handle_t, int, int,
                                           uint32_t));
void sgl_set_region(sgl_painter_handle_t painter, int x0, int y0, int x1,
                    int y1);
void sgl_draw_piexl_mono(sgl_painter_handle_t painter, int x, int y,
                         uint32_t color);
void sgl_draw_piexl_rgb565(sgl_painter_handle_t painter, int x, int y,
                           uint32_t color);
void sgl_draw_point(sgl_painter_handle_t painter, int x, int y,
                    uint32_t color);
void sgl_draw_hline(sgl_painter_handle_t painter, int x0, int x1, int y, uint32_t color);
void sgl_draw_vline(sgl_painter_handle_t painter, int y0, int y1, int x, uint32_t color);
// 功能函数

// 图型函数

// 字符函数

#endif
