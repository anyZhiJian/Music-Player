// #ifndef __GUI_H
// #define __GUI_H

// #include "glib.h"
// #include "guibase.h"

// typedef enum
// {
//     gui_signal_no = 0,
//     gui_signal_enter,
//     gui_signal_return,
//     gui_signal_left,
//     gui_signal_right,
//     gui_signal_up,
//     gui_signal_down,
// } gui_signal_t;

// typedef struct
// {
//     glib_painter_t* painter;
//     gui_signal_t signal;
//     list_t* list_canvas;
//     uint64_t tick;
//     uint32_t fps;
//     uint32_t frame;
//     void (* animation)(int* value, int target);
// } gui_t;

// void gui_Init(gui_t* gui, glib_painter_t* painter);
// void gui_TickInc(gui_t* gui);
// void gui_Proc(gui_t* gui);

// #endif
