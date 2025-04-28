// #ifndef __GUIBASE_H
// #define __GUIBASE_H

// #include "glib.h"
// #include "list.h"

// enum
// {
//     GUI_TYPE_HLINE = 0,
//     GUI_TYPE_VLINE,
//     GUI_TYPE_LINE,
//     GUI_TYPE_RECT,
//     GUI_TYPE_ROUNDRECT,
//     GUI_TYPE_CIRCLE,
//     GUI_TYPE_ELLIPSE,
//     GUI_TYPE_IMAGE,
//     GUI_TYPE_STRING,
//     GUI_TYPE_NUM,
// };

// typedef enum
// {
//     gui_visible = 0,
//     gui_invisible,
// } gui_visibility_t;

// typedef struct
// {
//     int x;
//     int y;
//     int width;
//     int height;
//     int r;
// } gui_size_t;

// typedef struct
// {
//     gui_size_t size;    // 当前值
//     gui_size_t target;  // 目标值
//     uint32_t color;     // 颜色
//     uint8_t status;     // 状态
//     uint8_t type;       // 类型
// } gui_widget_t;

// #define GUI_STATUS_VISIBILITY_MASK  0x01
// #define GUI_STATUS_FILLSTYLE_MASK   0x02
// #define GUI_STATUS_DIRECTION_MASK   0x0C
// #define GUI_STATUS_ALIGNMENT_MASK   0xF0

// /**
//   * status:
//   *
//   *   Bit: |-D7 D6 D5 D4-|----D3 D2----|------D1-----|-----D0-----|
//   * Value: |--alignment--|--direction--|--fillstyle--|-visibility-|
//   *
//   */

// typedef struct
// {
//     list_t* list;
//     node_t* self;
//     gui_visibility_t visibility;
// } gui_canvas_t;

// gui_canvas_t* gui_CreateCanvas(void);
// void gui_CleanCanvas(gui_canvas_t* canvas);
// void gui_SizeInit(gui_widget_t* widget, int x, int y, int width, int height, int r);

// gui_widget_t* gui_AddHLine(gui_canvas_t* canvas, int x, int y, int length, uint32_t color);
// gui_widget_t* gui_AddVLine(gui_canvas_t* canvas, int x, int y, int length, uint32_t color);
// gui_widget_t* gui_AddLine(gui_canvas_t* canvas, int x1, int y1, int x2, int y2, uint32_t color);
// gui_widget_t* gui_AddRect(gui_canvas_t* canvas, int x, int y, int width, int height, glib_fillstyle_t fillstyle, uint32_t color);
// gui_widget_t* gui_AddRoundrect(gui_canvas_t* canvas, int x, int y, int width, int height, int r, glib_fillstyle_t fillstyle, uint32_t color);
// gui_widget_t* gui_AddCircle(gui_canvas_t* canvas, int xc, int yc, int r, glib_fillstyle_t fillstyle, uint32_t color);
// gui_widget_t* gui_AddEllipse(gui_canvas_t* canvas, int xc, int yc, int r_x, int r_y, glib_fillstyle_t fillstyle, uint32_t color);
// gui_widget_t* gui_AddImage(gui_canvas_t* canvas, int x, int y, uint32_t width, uint32_t height, const uint8_t* image, uint32_t color);
// gui_widget_t* gui_AddString(gui_canvas_t* canvas, int x, int y, char* str, uint32_t length, uint32_t color);

// void gui_HideCanvas(gui_canvas_t* canvas, int hide);
// void gui_SetVisibility(gui_widget_t* widget, gui_visibility_t visibility);
// void gui_SetFillstyle(gui_widget_t* widget, glib_fillstyle_t fillstyle);
// void gui_SetDirection(gui_widget_t* widget, glib_dir_t dir);
// void gui_SetAlignment(gui_widget_t* widget, glib_align_t align);

// void gui_PaintCanvas(glib_painter_t* painter, gui_canvas_t* canvas);

// #endif
