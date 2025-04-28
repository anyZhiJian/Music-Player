#include "sgl.h"

void sgl_painter_init(sgl_painter_handle_t painter, void *buffer,
                      uint32_t buffer_size, uint32_t hor_res, uint32_t ver_res,
                      sgl_buffer_struct_t bs, sgl_mode_t mode)
{
    painter->buffer = buffer;
    painter->buffer_size = buffer_size;
    painter->hor_res = hor_res;
    painter->ver_res = ver_res;
    painter->mask.left = 0;
    painter->mask.right = hor_res;
    painter->mask.top = 0;
    painter->mask.bottom = ver_res;



    painter->buffer_width = hor_res;
    painter->buffer_height = ver_res;
    painter->offset_x = 0;
    painter->offset_y = 0;
    painter->page_start_row = 0;
    painter->page_width = ver_res;
    painter->mode = mode;

    if (mode == sgl_mode_page)
    {
        painter->buffer_height = buffer_size / hor_res;
        if (bs == sgl_bs_1bit)
            painter->buffer_height *= 8;
        painter->page_width = painter->buffer_height;
    }

    painter->font_info = font_get_font_info(font_Song_Typeface_8x16);
    painter->background = 0;
    painter->rotate = sgl_rotate_0;
    painter->frame_start_cb = NULL;
    painter->frame_end_cb = NULL;
    painter->draw_piexl = sgl_draw_piexl_mono;
}

void sgl_paint_start(sgl_painter_handle_t painter)
{
    if (painter->page_start_row == 0 && painter->frame_start_cb)
        painter->frame_start_cb();
    sgl_memset(painter->buffer, painter->background, painter->buffer_size);
}

void sgl_paint_end(sgl_painter_handle_t painter)
{
    painter->page_start_row += painter->page_width;
    if (painter->page_start_row >= painter->ver_res)
    {
        painter->page_start_row = 0;
        if (painter->frame_end_cb)
            painter->frame_end_cb();
    }
}

void sgl_display(sgl_painter_handle_t painter)
{
    sgl_paint_start(painter);
    if (painter->draw)
        painter->draw(painter);
    if (painter->flush)
        painter->flush(painter->buffer, painter->buffer_size);
    sgl_paint_end(painter);
}

void sgl_set_draw(sgl_painter_handle_t painter,
                  void (*draw)(sgl_painter_handle_t))
{
    painter->draw = draw;
}

void sgl_set_flush(sgl_painter_handle_t painter,
                   void (*flush)(void *, uint32_t))
{
    painter->flush = flush;
}

void sgl_set_draw_piexl(sgl_painter_handle_t painter,
                        void (*draw_piexl)(sgl_painter_handle_t, int, int,
                                           uint32_t))
{
    painter->draw_piexl = draw_piexl;
}

void sgl_set_frame_start_cb(sgl_painter_handle_t painter,
                            void (*frame_start_cb)(void))
{
    painter->frame_start_cb = frame_start_cb;
}

void sgl_set_frame_end_cb(sgl_painter_handle_t painter,
                          void (*frame_end_cb)(void))
{
    painter->frame_end_cb = frame_end_cb;
}

void sgl_set_region(sgl_painter_handle_t painter, int x0, int y0, int x1,
                    int y1)
{
    painter->offset_x = x0;
    painter->offset_y = y0;
    painter->buffer_width = x1 - x0 + 1;
    painter->buffer_height = y1 - y0 + 1;
}

void sgl_set_font(sgl_painter_handle_t painter, font_t font)
{
    painter->font_info = font_get_font_info(font);
}

void sgl_set_background(sgl_painter_handle_t painter, uint32_t background)
{
    painter->background = background;
}

void sgl_set_rotation(sgl_painter_handle_t painter, sgl_rotate_t rotate)
{
    painter->rotate = rotate;
}

void sgl_align(int *x, int *y, int width, int height, sgl_align_t align)
{
    switch (align)
    {
    case sgl_align_up_left:
        break;
    case sgl_align_up_right:
        *x -= width - 1;
        break;
    case sgl_align_down_left:
        *y -= height - 1;
        break;
    case sgl_align_down_right:
        *x -= width - 1;
        *y -= height - 1;
        break;
    case sgl_align_center:
        *x -= width / 2;
        *y -= height / 2;
        break;
    case sgl_align_up_center:
        *x -= width / 2;
        break;
    case sgl_align_down_center:
        *x -= width / 2;
        *y -= height - 1;
        break;
    case sgl_align_left_center:
        *y -= height / 2;
        break;
    case sgl_align_right_center:
        *x -= width - 1;
        *y -= height / 2;
        break;
    default:
        break;
    }
}

void sgl_rotated2original(int *x, int *y, uint32_t hor_res, uint32_t ver_res,
                          sgl_rotate_t rotate)
{
    int temp = *x;
    if (rotate == sgl_rotate_90)
    {
        *x = (hor_res - 1) - *y;
        *y = temp;
    }
    else if (rotate == sgl_rotate_270)
    {
        *x = *y;
        *y = (ver_res - 1) - temp;
    }
    else if (rotate == sgl_rotate_180)
    {
        *x = (hor_res - 1) - *x;
        *y = (ver_res - 1) - *y;
    }
}

void sgl_original2rotated(int *x, int *y, uint32_t hor_res, uint32_t ver_res,
                          sgl_rotate_t rotate)
{
    int temp = *x;
    if (rotate == sgl_rotate_90)
    {
        *x = *y;
        *y = (hor_res - 1) - temp;
    }
    else if (rotate == sgl_rotate_270)
    {
        *x = (ver_res - 1) - *y;
        *y = temp;
    }
    else if (rotate == sgl_rotate_180)
    {
        *x = (hor_res - 1) - *x;
        *y = (ver_res - 1) - *y;
    }
}

static void sgl_remap(sgl_painter_handle_t painter, int *x, int *y)
{
    if (painter->rotate)
        sgl_rotated2original(x, y, painter->hor_res, painter->ver_res,
                             painter->rotate);

    if (painter->mode == sgl_mode_page)
        *y -= painter->page_start_row;

    if (painter->mode == sgl_mode_Partial)
    {
        *x -= painter->offset_x;
        *y -= painter->offset_y;
    }
}

void sgl_draw_piexl_mono(sgl_painter_handle_t painter, int x, int y,
                         uint32_t color)
{
    switch (color)
    {
    case color_mono_light:
        painter->buffer[(y / 8) * painter->buffer_width + x] |= (1 << (y % 8));
        break;
    case color_mono_dark:
        painter->buffer[(y / 8) * painter->buffer_width + x] &= ~(1 << (y % 8));
        break;
    case color_mono_inverse:
        painter->buffer[(y / 8) * painter->buffer_width + x] ^= (1 << (y % 8));
        break;
    }
}

void sgl_draw_piexl_rgb565(sgl_painter_handle_t painter, int x, int y,
                           uint32_t color)
{
    painter->buffer[(x + painter->buffer_width * y) * 2] = color >> 8;
    painter->buffer[(x + painter->buffer_width * y) * 2 + 1] = color;
}

inline static int sgl_mask(sgl_painter_handle_t painter, int *left, int *right, int *top, int *bottom)
{
    sgl_rect_t mask = painter->mask;
    if(*left > mask.right || *right < mask.left || *top > mask.bottom || *bottom < mask.top)
        return -1;
    if(*left < mask.left)
    *left = mask.left;
    if(*right > mask.right)
    *right = mask.right;
    if(*top < mask.top)
    *top = mask.top;
    if(*bottom > mask.bottom)
    *bottom = mask.bottom;
    return 0;
}

void sgl_draw_point(sgl_painter_handle_t painter, int x, int y,
                    uint32_t color)
{
    if(sgl_mask(painter, &x, &x, &y, &y))
        return;
    sgl_remap(painter, &x, &y);
    painter->draw_piexl(painter, x, y, color);
}

inline static void sgl_draw_original_hline(sgl_painter_handle_t painter, int x0, int x1, int y, uint32_t color)
{
    int i;
    for(i = x0; i <= x1; ++i)
    {
        painter->draw_piexl(painter, i, y, color);
    }
}

inline static void sgl_draw_original_vline(sgl_painter_handle_t painter, int y0, int y1, int x, uint32_t color)
{
    int i;
    for(i = y0; i <= y1; ++i)
    {
        painter->draw_piexl(painter, x, i, color);
    }
}

void sgl_draw_hline(sgl_painter_handle_t painter, int x0, int x1, int y, uint32_t color)
{
    int x0_remap = x0;
    int x1_remap = x1;
    int y0_remap = y;
    int y1_remap = y;
    if(sgl_mask(painter, &x0_remap, &x1_remap, &y0_remap, &y1_remap))
        return;
    sgl_remap(painter, &x0_remap, &y0_remap);
    sgl_remap(painter, &x1_remap, &y1_remap);
    switch(painter->rotate)
    {
        case sgl_rotate_0:sgl_draw_original_hline(painter, x0_remap, x1_remap, y0_remap, color);break;
        case sgl_rotate_90:sgl_draw_original_vline(painter, y0_remap, y1_remap, x0_remap, color);break;
        case sgl_rotate_180:sgl_draw_original_hline(painter, x1_remap, x0_remap, y0_remap, color);break;
        case sgl_rotate_270:sgl_draw_original_vline(painter, y1_remap, y0_remap, x0_remap, color);break;
        default:break;
    }
}

void sgl_draw_vline(sgl_painter_handle_t painter, int y0, int y1, int x, uint32_t color)
{
    int x0_remap = x;
    int x1_remap = x;
    int y0_remap = y0;
    int y1_remap = y1;
    if(sgl_mask(painter, &x0_remap, &x1_remap, &y0_remap, &y1_remap))
        return;
    sgl_remap(painter, &x0_remap, &y0_remap);
    sgl_remap(painter, &x1_remap, &y1_remap);
    switch(painter->rotate)
    {
        case sgl_rotate_0:sgl_draw_original_vline(painter, y0_remap, y1_remap, x0_remap, color);break;
        case sgl_rotate_90:sgl_draw_original_hline(painter, x1_remap, x0_remap, y0_remap, color);break;
        case sgl_rotate_180:sgl_draw_original_vline(painter, y1_remap, y0_remap, x0_remap, color);break;
        case sgl_rotate_270:sgl_draw_original_hline(painter, x0_remap, x1_remap, y0_remap, color);break;
        default:break;
    }
}

// void sgl_draw_hline_with_length(sgl_painter_handle_t painter, int x, int y, int length,
//                     uint32_t color)
// {

// }

// void sgl_draw_vline_with_length(sgl_painter_handle_t painter, int x, int y, int length,
//                     uint32_t color)
// {

// }

// void sgl_draw_line(sgl_painter_handle_t painter, int x0, int y0, int x1, int y1,
//                    uint32_t color)
// {
//     int i;
//     int error_x = 0, error_y = 0, delta_x, delta_y, distance, inc_x, inc_y,
//         x = x0, y = y0;

//     delta_x = x1 - x0;
//     delta_y = y1 - y0;
//     delta_x = (delta_x ^ (delta_x >> 31)) - (delta_x >> 31); // 求delta_x绝对值
//     delta_y = (delta_y ^ (delta_y >> 31)) - (delta_y >> 31); // 求delta_y绝对值

//     if (delta_x == 0)
//         inc_x = 0;
//     else
//         inc_x = x1 > x0 ? 1 : -1;
//     if (delta_y == 0)
//         inc_y = 0;
//     else
//         inc_y = y1 > y0 ? 1 : -1;
//     distance = delta_x > delta_y ? delta_x : delta_y;

//     for (i = 0; i < distance; ++i) // 不绘制终点(x1, y1)
//     {
//         error_x += delta_x;
//         error_y += delta_y;
//         if (error_x > distance)
//         {
//             error_x -= distance;
//             x += inc_x;
//         }
//         if (error_y > distance)
//         {
//             error_y -= distance;
//             y += inc_y;
//         }
//         sgl_draw_point(painter, x, y, color);
//     }
// }

// void sgl_draw_polyline(sgl_painter_handle_t painter, int *x, int *y, int size,
//                        uint32_t color)
// {
//     int i;
//     for (i = 0; i < size - 1; ++i) // 不绘制终点(x[size - 1], y[size - 1])
//         sgl_draw_line(painter, x[i], y[i], x[i + 1], y[i + 1], color);
// }

// void sgl_draw_polygon(sgl_painter_handle_t painter, int *x, int *y, int size,
//                       uint32_t color)
// {
//     if (size < 3)
//         return;
//     sgl_draw_polyline(painter, x, y, size, color);
//     sgl_draw_line(painter, x[size - 1], y[size - 1], x[0], y[0], color);
// }

// void sgl_draw_rect(sgl_painter_handle_t painter, int x, int y, int width,
//                    int height, sgl_fillstyle_t fillstyle, uint32_t color)
// {
//     if (fillstyle == sgl_fillstyle_outlined)
//     {
//         if (height == 1)
//             sgl_draw_hline(painter, x, y, width, color);
//         if (height == 2)
//         {
//             sgl_draw_hline(painter, x, y, width, color);
//             sgl_draw_hline(painter, x, y + 1, width, color);
//         }
//         else if (height > 2 && width == 1)
//         {
//             sgl_draw_hline(painter, x, y, width, color);
//             sgl_draw_hline(painter, x, y + height - 1, width, color);
//             sgl_draw_vline(painter, x, y + 1, height - 2, color);
//         }
//         else if (height > 2 && width > 1)
//         {
//             sgl_draw_hline(painter, x, y, width, color);
//             sgl_draw_hline(painter, x, y + height - 1, width, color);
//             sgl_draw_vline(painter, x, y + 1, height - 2, color);
//             sgl_draw_vline(painter, x + width - 1, y + 1, height - 2, color);
//         }
//     }
//     else
//     {
//         int i;
//         for (i = 0; i < height; ++i)
//         {
//             sgl_draw_hline(painter, x, y + i, width, color);
//         }
//     }
// }

// void sgl_draw_circle_section(sgl_painter_handle_t painter, int xc, int yc,
//                              int r, sgl_circlemode_t circlemode,
//                              sgl_fillstyle_t fillstyle, uint32_t color)
// {
//     /*
//     +-----------> X
//     |   6  7
//     | 5      8
//     | 4      1
//     |   3  2
//     v
//     Y
//     */
//     if (r < 2) // 半径小于2时不需要绘制圆弧
//         return;

//     int x = 0, y = r, d;
//     d = 3 - 2 * r;

//     if (fillstyle == sgl_fillstyle_outlined)
//     {
//         while (1)
//         {
//             if (d < 0)
//             {
//                 d = d + 4 * x + 6;
//             }
//             else
//             {
//                 d = d + 4 * (x - y) + 10;
//                 y--;
//             }
//             x++;
//             if (x < y)
//             {
//                 if (circlemode & sgl_circlemode_lower_right)
//                 {
//                     sgl_draw_point(painter, xc + y, yc + x, color); // 1
//                     sgl_draw_point(painter, xc + x, yc + y, color); // 2
//                 }
//                 if (circlemode & sgl_circlemode_lower_left)
//                 {
//                     sgl_draw_point(painter, xc - x, yc + y, color); // 3
//                     sgl_draw_point(painter, xc - y, yc + x, color); // 4
//                 }
//                 if (circlemode & sgl_circlemode_upper_left)
//                 {
//                     sgl_draw_point(painter, xc - y, yc - x, color); // 5
//                     sgl_draw_point(painter, xc - x, yc - y, color); // 6
//                 }
//                 if (circlemode & sgl_circlemode_upper_right)
//                 {
//                     sgl_draw_point(painter, xc + x, yc - y, color); // 7
//                     sgl_draw_point(painter, xc + y, yc - x, color); // 8
//                 }
//             }
//             else
//                 break;
//         }

//         if (x == y)
//         {
//             if (circlemode & sgl_circlemode_lower_right)
//                 sgl_draw_point(painter, xc + y, yc + x, color); // 1, 2
//             if (circlemode & sgl_circlemode_lower_left)
//                 sgl_draw_point(painter, xc - x, yc + y, color); // 3, 4
//             if (circlemode & sgl_circlemode_upper_left)
//                 sgl_draw_point(painter, xc - y, yc - x, color); // 5, 6
//             if (circlemode & sgl_circlemode_upper_right)
//                 sgl_draw_point(painter, xc + x, yc - y, color); // 7, 8
//         }
//     }
//     else
//     {
//         while (1)
//         {
//             if (d < 0)
//             {
//                 d = d + 4 * x + 6;
//             }
//             else
//             {
//                 d = d + 4 * (x - y) + 10;
//                 y--;
//             }
//             x++;
//             if (x < y)
//             {
//                 if (circlemode & sgl_circlemode_lower_right)
//                 {
//                     sgl_draw_hline(painter, xc + x + 1, yc + x, y - x,
//                                    color); // 1
//                     sgl_draw_vline(painter, xc + x, yc + x + 1, y - x,
//                                    color); // 2
//                 }
//                 if (circlemode & sgl_circlemode_lower_left)
//                 {
//                     sgl_draw_vline(painter, xc - x, yc + x + 1, y - x,
//                                    color);                                 // 3
//                     sgl_draw_hline(painter, xc - y, yc + x, y - x, color); // 4
//                 }
//                 if (circlemode & sgl_circlemode_upper_left)
//                 {
//                     sgl_draw_hline(painter, xc - y, yc - x, y - x, color); // 5
//                     sgl_draw_vline(painter, xc - x, yc - y, y - x, color); // 6
//                 }
//                 if (circlemode & sgl_circlemode_upper_right)
//                 {
//                     sgl_draw_vline(painter, xc + x, yc - y, y - x, color); // 7
//                     sgl_draw_hline(painter, xc + x + 1, yc - x, y - x,
//                                    color); // 8
//                 }
//             }
//             else
//                 break;
//         }

//         int i;
//         if (x == y)
//         {
//             if (circlemode & sgl_circlemode_lower_right)
//                 for (i = 1; i <= x; i++)
//                     sgl_draw_point(painter, xc + i, yc + i, color); // 1, 2
//             if (circlemode & sgl_circlemode_lower_left)
//                 for (i = 1; i <= x; i++)
//                     sgl_draw_point(painter, xc - i, yc + i, color); // 3, 4
//             if (circlemode & sgl_circlemode_upper_left)
//                 for (i = 1; i <= x; i++)
//                     sgl_draw_point(painter, xc - i, yc - i, color); // 5, 6
//             if (circlemode & sgl_circlemode_upper_right)
//                 for (i = 1; i <= x; i++)
//                     sgl_draw_point(painter, xc + i, yc - i, color); // 7, 8
//         }
//         else
//         {
//             if (circlemode & sgl_circlemode_lower_right)
//                 for (i = 1; i < x; i++)
//                     sgl_draw_point(painter, xc + i, yc + i, color); // 1, 2
//             if (circlemode & sgl_circlemode_lower_left)
//                 for (i = 1; i < x; i++)
//                     sgl_draw_point(painter, xc - i, yc + i, color); // 3, 4
//             if (circlemode & sgl_circlemode_upper_left)
//                 for (i = 1; i < x; i++)
//                     sgl_draw_point(painter, xc - i, yc - i, color); // 5, 6
//             if (circlemode & sgl_circlemode_upper_right)
//                 for (i = 1; i < x; i++)
//                     sgl_draw_point(painter, xc + i, yc - i, color); // 7, 8
//         }
//     }
// }

// void sgl_DrawCircle(sgl_painter_handle_t painter, int xc, int yc, int r,
//                     sgl_fillstyle_t fillstyle, uint32_t color)
// {
//     if (fillstyle == sgl_fillstyle_outlined)
//     {
//         sgl_draw_point(painter, xc + r, yc, color); // 1, 2
//         sgl_draw_point(painter, xc, yc + r, color); // 3, 4
//         sgl_draw_point(painter, xc - r, yc, color); // 5, 6
//         sgl_draw_point(painter, xc, yc - r, color); // 7, 8
//     }
//     else
//     {
//         sgl_draw_point(painter, xc, yc, color); // (xc, yc)

//         sgl_draw_hline(painter, xc + 1, yc, r, color); // 8, 1
//         sgl_draw_vline(painter, xc, yc + 1, r, color); // 2, 3
//         sgl_draw_hline(painter, xc - r, yc, r, color); // 4, 5
//         sgl_draw_vline(painter, xc, yc - r, r, color); // 6, 7
//     }

//     sgl_draw_circle_section(painter, xc, yc, r, circlemode_all, fillstyle,
//                             color);
// }

// void sgl_draw_ellipse_section(sgl_painter_handle_t painter, int xc, int yc,
//                               int r_x, int r_y, sgl_circlemode_t circlemode,
//                               sgl_fillstyle_t fillstyle, uint32_t color)
// {
//     if (r_x < 1 || r_y < 1)
//         return;

//     int x = 0, y = r_y, d1, d2;
//     int rx_2 = r_x * r_x;
//     int ry_2 = r_y * r_y;

//     if (fillstyle == sgl_fillstyle_outlined)
//     {
//         d1 = 4 * ry_2 + rx_2 * (1 - 4 * r_y);
//         while (2 * ry_2 * (x + 1) < rx_2 * (2 * y - 1))
//         {
//             if (d1 <= 0)
//             {
//                 d1 += 4 * ry_2 * (2 * x + 3);
//                 ++x;
//             }
//             else
//             {
//                 d1 += 4 * (ry_2 * (2 * x + 3) + rx_2 * (-2 * y + 2));
//                 ++x;
//                 --y;
//             }

//             if (circlemode & sgl_circlemode_lower_right)
//                 sgl_draw_point(painter, xc + x, yc + y, color);
//             if (circlemode & sgl_circlemode_upper_right)
//                 sgl_draw_point(painter, xc + x, yc - y, color);
//             if (circlemode & sgl_circlemode_lower_left)
//                 sgl_draw_point(painter, xc - x, yc + y, color);
//             if (circlemode & sgl_circlemode_upper_left)
//                 sgl_draw_point(painter, xc - x, yc - y, color);
//         }

//         d2 = ry_2 * (2 * x + 1) * (2 * x + 1) +
//              4 * (rx_2 * (y - 1) * (y - 1) - rx_2 * ry_2);
//         while (y > 0)
//         {
//             if (d2 <= 0)
//             {
//                 d2 += 4 * (ry_2 * (2 * x + 2) + rx_2 * (-2 * y + 3));
//                 ++x;
//                 --y;
//             }
//             else
//             {
//                 d2 += 4 * (rx_2 * (-2 * y + 3));
//                 --y;
//             }

//             if (circlemode & sgl_circlemode_lower_right)
//                 sgl_draw_point(painter, xc + x, yc + y, color);
//             if (circlemode & sgl_circlemode_upper_right)
//                 sgl_draw_point(painter, xc + x, yc - y, color);
//             if (circlemode & sgl_circlemode_lower_left)
//                 sgl_draw_point(painter, xc - x, yc + y, color);
//             if (circlemode & sgl_circlemode_upper_left)
//                 sgl_draw_point(painter, xc - x, yc - y, color);
//         }
//     }
//     else
//     {
//         d1 = 4 * ry_2 + rx_2 * (1 - 4 * r_y);
//         while (2 * ry_2 * (x + 1) < rx_2 * (2 * y - 1))
//         {
//             if (d1 <= 0)
//             {
//                 d1 += 4 * ry_2 * (2 * x + 3);
//                 ++x;
//                 if (circlemode & sgl_circlemode_lower_right)
//                     sgl_draw_point(painter, xc + x, yc + y, color);
//                 if (circlemode & sgl_circlemode_upper_right)
//                     sgl_draw_point(painter, xc + x, yc - y, color);
//                 if (circlemode & sgl_circlemode_lower_left)
//                     sgl_draw_point(painter, xc - x, yc + y, color);
//                 if (circlemode & sgl_circlemode_upper_left)
//                     sgl_draw_point(painter, xc - x, yc - y, color);
//             }
//             else
//             {
//                 d1 += 4 * (ry_2 * (2 * x + 3) + rx_2 * (-2 * y + 2));
//                 ++x;
//                 --y;
//                 if (circlemode & sgl_circlemode_lower_right)
//                     sgl_draw_hline(painter, xc + 1, yc + y, x, color);
//                 if (circlemode & sgl_circlemode_upper_right)
//                     sgl_draw_hline(painter, xc + 1, yc - y, x, color);
//                 if (circlemode & sgl_circlemode_lower_left)
//                     sgl_draw_hline(painter, xc - x, yc + y, x, color);
//                 if (circlemode & sgl_circlemode_upper_left)
//                     sgl_draw_hline(painter, xc - x, yc - y, x, color);
//             }
//         }

//         d2 = ry_2 * (2 * x + 1) * (2 * x + 1) +
//              4 * (rx_2 * (y - 1) * (y - 1) - rx_2 * ry_2);
//         while (y > 0)
//         {
//             if (d2 <= 0)
//             {
//                 d2 += 4 * (ry_2 * (2 * x + 2) + rx_2 * (-2 * y + 3));
//                 ++x;
//                 --y;
//             }
//             else
//             {
//                 d2 += 4 * (rx_2 * (-2 * y + 3));
//                 --y;
//             }

//             if (circlemode & sgl_circlemode_lower_right)
//                 sgl_draw_hline(painter, xc + 1, yc + y, x, color);
//             if (circlemode & sgl_circlemode_upper_right)
//                 sgl_draw_hline(painter, xc + 1, yc - y, x, color);
//             if (circlemode & sgl_circlemode_lower_left)
//                 sgl_draw_hline(painter, xc - x, yc + y, x, color);
//             if (circlemode & sgl_circlemode_upper_left)
//                 sgl_draw_hline(painter, xc - x, yc - y, x, color);
//         }
//     }
// }

// void sgl_draw_ellipse(sgl_painter_handle_t painter, int xc, int yc, int r_x,
//                       int r_y, sgl_fillstyle_t fillstyle, uint32_t color)
// {
//     if (fillstyle == sgl_fillstyle_outlined)
//     {
//         sgl_draw_point(painter, xc + r_x, yc, color); // 1, 2
//         sgl_draw_point(painter, xc, yc + r_y, color); // 3, 4
//         sgl_draw_point(painter, xc - r_x, yc, color); // 5, 6
//         sgl_draw_point(painter, xc, yc - r_y, color); // 7, 8
//     }
//     else
//     {
//         sgl_draw_point(painter, xc, yc, color); // (xc, yc)

//         sgl_draw_hline(painter, xc + 1, yc, r_x, color);   // 8, 1
//         sgl_draw_vline(painter, xc, yc + 1, r_y, color);   // 2, 3
//         sgl_draw_hline(painter, xc - r_x, yc, r_x, color); // 4, 5
//         sgl_draw_vline(painter, xc, yc - r_y, r_y, color); // 6, 7
//     }

//     sgl_draw_ellipse_section(painter, xc, yc, r_x, r_y, circlemode_all,
//                              fillstyle, color);
// }

// void sgl_draw_round_rect(sgl_painter_handle_t painter, int x, int y, int width,
//                          int height, int r, sgl_fillstyle_t fillstyle,
//                          uint32_t color)
// {
//     int width_rect = width - 2 * r;
//     int height_rect = height - 2 * r;

//     if (width_rect < 0 || height_rect < 0)
//         return;

//     if (fillstyle == sgl_fillstyle_outlined)
//     {
//         sgl_draw_hline(painter, x + r, y, width_rect, color);
//         sgl_draw_hline(painter, x + r, y + height - 1, width_rect, color);
//         sgl_draw_vline(painter, x, y + r, height_rect, color);
//         sgl_draw_vline(painter, x + width - 1, y + r, height_rect, color);
//     }
//     else
//     {
//         sgl_draw_rect(painter, x + r, y + r, width_rect, height_rect, fillstyle,
//                       color);

//         sgl_draw_rect(painter, x + r, y, width_rect, r, fillstyle, color);
//         sgl_draw_rect(painter, x + r, y + height - r, width_rect, r, fillstyle,
//                       color);
//         sgl_draw_rect(painter, x, y + r, r, height_rect, fillstyle, color);
//         sgl_draw_rect(painter, x + width - r, y + r, r, height_rect, fillstyle,
//                       color);
//     }

//     sgl_draw_circle_section(painter, x + r, y + r, r, sgl_circlemode_upper_left,
//                             fillstyle, color);
//     sgl_draw_circle_section(painter, x + r, y + height - r - 1, r,
//                             sgl_circlemode_lower_left, fillstyle, color);
//     sgl_draw_circle_section(painter, x + width - r - 1, y + r, r,
//                             sgl_circlemode_upper_right, fillstyle, color);
//     sgl_draw_circle_section(painter, x + width - r - 1, y + height - r - 1, r,
//                             sgl_circlemode_lower_right, fillstyle, color);
// }

// void sgl_show_mono_image(sgl_painter_handle_t painter, int x, int y, int width,
//                          int height, const uint8_t *image, sgl_dir_t dir,
//                          uint32_t color)
// {
//     int i = 0;
//     int j = 0;

//     if (dir == sgl_dir_up)
//     {
//         for (i = 0; i < width; ++i)
//         {
//             for (j = 0; j < height; ++j)
//             {
//                 if ((image[i + (j / 8 * width)] & (1 << (j % 8))))
//                 {
//                     sgl_draw_point(painter, x + i, y + j, color);
//                 }
//             }
//         }
//     }
//     else if (dir == sgl_dir_right)
//     {
//         for (i = 0; i < width; ++i)
//         {
//             for (j = 0; j < height; ++j)
//             {
//                 if ((image[i + (j / 8 * width)] & (1 << (j % 8))))
//                 {
//                     sgl_draw_point(painter, x + height - j - 1, y + i, color);
//                 }
//             }
//         }
//     }
//     else if (dir == sgl_dir_left)
//     {
//         for (i = 0; i < width; ++i)
//         {
//             for (j = 0; j < height; ++j)
//             {
//                 if ((image[i + (j / 8 * width)] & (1 << (j % 8))))
//                 {
//                     sgl_draw_point(painter, x + j, y + width - i - 1, color);
//                 }
//             }
//         }
//     }
//     else if (dir == sgl_dir_down)
//     {
//         for (i = 0; i < width; ++i)
//         {
//             for (j = 0; j < height; ++j)
//             {
//                 if ((image[i + (j / 8 * width)] & (1 << (j % 8))))
//                 {
//                     sgl_draw_point(painter, x + width - i - 1,
//                                    y + height - j - 1, color);
//                 }
//             }
//         }
//     }
// }

// void sgl_show_string(sgl_painter_handle_t painter, int x, int y,
//                      const char *str, int length, sgl_align_t align,
//                      sgl_dir_t dir, uint32_t color)
// {
//     int i = 0;
//     int fontwidth = painter->font_info.width;
//     int fontheight = painter->font_info.height;

//     if (dir == sgl_dir_up)
//     {
//         sgl_align(&x, &y, fontwidth * length, fontheight, align);
//         for (i = 0; i < length; ++i)
//         {
//             sgl_show_mono_image(
//                 painter, x + i * fontwidth, y, fontwidth, fontheight,
//                 &painter->font_info
//                      .font[(str[i] - ' ') * painter->font_info.fontsize],
//                 dir, color);
//         }
//     }
//     else if (dir == sgl_dir_right)
//     {
//         sgl_align(&x, &y, fontheight, fontwidth * length, align);
//         for (i = 0; i < length; ++i)
//         {
//             sgl_show_mono_image(
//                 painter, x, y + i * fontwidth, fontwidth, fontheight,
//                 &painter->font_info
//                      .font[(str[i] - ' ') * painter->font_info.fontsize],
//                 dir, color);
//         }
//     }
//     else if (dir == sgl_dir_left)
//     {
//         sgl_align(&x, &y, fontheight, fontwidth * length, align);
//         y += (length - 1) * fontwidth;
//         for (i = 0; i < length; ++i)
//         {
//             sgl_show_mono_image(
//                 painter, x, y - i * fontwidth, fontwidth, fontheight,
//                 &painter->font_info
//                      .font[(str[i] - ' ') * painter->font_info.fontsize],
//                 dir, color);
//         }
//     }
//     else if (dir == sgl_dir_down)
//     {
//         sgl_align(&x, &y, fontwidth * length, fontheight, align);
//         x += (length - 1) * fontwidth;
//         for (i = 0; i < length; ++i)
//         {
//             sgl_show_mono_image(
//                 painter, x - i * fontwidth, y, fontwidth, fontheight,
//                 &painter->font_info
//                      .font[(str[i] - ' ') * painter->font_info.fontsize],
//                 dir, color);
//         }
//     }
// }

// int sgl_show_format_string(sgl_painter_handle_t painter, int x, int y,
//                            sgl_align_t align, sgl_dir_t dir, uint32_t color,
//                            const char *format, ...)
// {
//     char buffer[sgl_FORMAT_STRING_BUFFERSIZE];
//     va_list args;
//     va_start(args, format);
//     int length = vsnprintf(buffer, sgl_FORMAT_STRING_BUFFERSIZE, format, args);
//     va_end(args);
//     sgl_show_string(painter, x, y, buffer, length, align, dir, color);
//     return length;
// }
