// #include "gui.h"

// void gui_Init(gui_t* gui, glib_painter_t* painter)
// {
//     gui->painter = painter;
//     gui->signal = gui_signal_no;
// }

// void gui_AddCanvas(gui_t* gui, gui_canvas_t* canvas)
// {
//     insert_node(gui->list_canvas, canvas->self, -1);
// }

// void gui_TickInc(gui_t* gui)
// {
//     ++gui->tick;
// }

// void gui_Animation(gui_t* gui, gui_canvas_t* canvas)
// {
//     if(gui->animation)
//     {
//         node_t* temp = canvas->list->head;
//         gui_widget_t* widget;
//         while(temp)
//         {
//             widget = temp->data;
//             temp = temp->next;

//             // gui->animation(widget->size.x, widget->target.x);
//             // gui->animation(widget->size.x, widget->target.x);
//             // gui->animation(widget->size.x, widget->target.x);
//             // gui->animation(widget->size.x, widget->target.x);
//             // gui->animation(widget->size.x, widget->target.x);
//         }
//     }
// }

// static void gui_SignalProc(gui_t* gui)
// {
    
// }

// static void gui_AnimationProc(gui_t* gui)
// {
//     node_t* temp = gui->list_canvas->head;
//     while(temp)
//     {
//         gui_canvas_t* canvas = temp->data;
//         temp = temp->next;
        
//         if(canvas->visibility == gui_visible)
//         {
//            gui_Animation(gui, canvas);
//         }
//     }
// }

// static void gui_PaintProc(gui_t* gui)
// {
//     node_t* temp = gui->list_canvas->head;
//     while(temp)
//     {
//         gui_canvas_t* canvas = temp->data;
//         temp = temp->next;
        
//         if(canvas->visibility == gui_visible)
//         {
//             gui_PaintCanvas(gui->painter, canvas);
//         }
//     }
// }

// void gui_Proc(gui_t* gui)
// {
//     if(gui->signal)
//     {
//         gui_SignalProc(gui);
//         gui_AnimationProc(gui);
//         gui_PaintProc(gui);
//     }
// }
