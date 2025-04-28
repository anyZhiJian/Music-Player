// #include "guibase.h"
// #include "string.h"

// gui_canvas_t* gui_CreateCanvas(void)
// {
//     node_t* node = create_node(sizeof(gui_canvas_t));
//     if(node)
//     {
//         gui_canvas_t* canvas = node->data;
//         canvas->list = create_list();
//         if(canvas->list)
//         {
//             canvas->self = node;
//             return canvas;
//         }
//     }
//     return NULL;
// }

// void gui_CleanCanvas(gui_canvas_t* canvas)
// {
//     delete_list(canvas->list);
// }

// void gui_SizeInit(gui_widget_t* widget, int x, int y, int width, int height, int r)
// {
//     widget->size.x = x;
//     widget->size.y = y;
//     widget->size.width = width;
//     widget->size.height = height;
//     widget->size.r = r;
//     widget->target =  widget->size;
// }

// gui_widget_t* gui_AddHLine(gui_canvas_t* canvas, int x, int y, int length, uint32_t color)
// {
//     node_t* node = create_node(sizeof(gui_widget_t));
//     if(node)
//     {
//         gui_widget_t* widget = node->data;
//         widget->type = GUI_TYPE_HLINE;
//         widget->color = color;
//         gui_SizeInit(widget, x, y, length, 0, 0);
//         insert_node(canvas->list, node, -1);
//         return widget;
//     }
//     return NULL;
// }

// gui_widget_t* gui_AddVLine(gui_canvas_t* canvas, int x, int y, int length, uint32_t color)
// {
//     node_t* node = create_node(sizeof(gui_widget_t));
//     if(node)
//     {
//         gui_widget_t* widget = node->data;
//         widget->type = GUI_TYPE_VLINE;
//         widget->color = color;
//         gui_SizeInit(widget, x, y, length, 0, 0);
//         insert_node(canvas->list, node, -1);
//         return widget;
//     }
//     return NULL;
// }

// gui_widget_t* gui_AddLine(gui_canvas_t* canvas, int x1, int y1, int x2, int y2, uint32_t color)
// {
//     node_t* node = create_node(sizeof(gui_widget_t));
//     if(node)
//     {
//         gui_widget_t* widget = node->data;
//         widget->type = GUI_TYPE_LINE;
//         widget->color = color;
//         gui_SizeInit(widget, x1, y1, x2, y2, 0);
//         insert_node(canvas->list, node, -1);
//         return widget;
//     }
//     return NULL;
// }

// gui_widget_t* gui_AddRect(gui_canvas_t* canvas, int x, int y, int width, int height, glib_fillstyle_t fillstyle, uint32_t color)
// {
//     node_t* node = create_node(sizeof(gui_widget_t));
//     if(node)
//     {
//         gui_widget_t* widget = node->data;
//         widget->type = GUI_TYPE_RECT;
//         widget->color = color;
//         gui_SizeInit(widget, x, y, width, height, 0);
//         widget->status |= fillstyle << 1;
//         insert_node(canvas->list, node, -1);
//         return widget;
//     }
//     return NULL;
// }

// gui_widget_t* gui_AddRoundrect(gui_canvas_t* canvas, int x, int y, int width, int height, int r, glib_fillstyle_t fillstyle, uint32_t color)
// {
//     node_t* node = create_node(sizeof(gui_widget_t));
//     if(node)
//     {
//         gui_widget_t* widget = node->data;
//         widget->type = GUI_TYPE_ROUNDRECT;
//         widget->color = color;
//         gui_SizeInit(widget, x, y, width, height, r);
//         widget->status |= fillstyle << 1;
//         insert_node(canvas->list, node, -1);
//         return widget;
//     }
//     return NULL;
// }

// gui_widget_t* gui_AddCircle(gui_canvas_t* canvas, int xc, int yc, int r, glib_fillstyle_t fillstyle, uint32_t color)
// {
//     node_t* node = create_node(sizeof(gui_widget_t));
//     if(node)
//     {
//         gui_widget_t* widget = node->data;
//         widget->type = GUI_TYPE_CIRCLE;
//         widget->color = color;
//         gui_SizeInit(widget, xc, yc, 0, 0, r);
//         widget->status |= fillstyle << 1;
//         insert_node(canvas->list, node, -1);
//         return widget;
//     }
//     return NULL;
// }

// gui_widget_t* gui_AddEllipse(gui_canvas_t* canvas, int xc, int yc, int r_x, int r_y, glib_fillstyle_t fillstyle, uint32_t color)
// {
//     node_t* node = create_node(sizeof(gui_widget_t));
//     if(node)
//     {
//         gui_widget_t* widget = node->data;
//         widget->type = GUI_TYPE_ELLIPSE;
//         widget->color = color;
//         gui_SizeInit(widget, xc, yc, r_x, r_y, 0);
//         widget->status |= fillstyle << 1;
//         insert_node(canvas->list, node, -1);
//         return widget;
//     }
//     return NULL;
// }

// gui_widget_t* gui_AddImage(gui_canvas_t* canvas, int x, int y, uint32_t width, uint32_t height, const uint8_t* image, uint32_t color)
// {
//     node_t* node = create_node(sizeof(gui_widget_t));
//     if(node)
//     {
//         gui_widget_t* widget = node->data;
//         widget->type = GUI_TYPE_STRING;
//         widget->color = color;
//         gui_SizeInit(widget, x, y, width, height, (int)image);
//         insert_node(canvas->list, node, -1);
//         return widget;
//     }
//     return NULL;
// }

// gui_widget_t* gui_AddString(gui_canvas_t* canvas, int x, int y, char* str, uint32_t length, uint32_t color)
// {
//     node_t* node = create_node(sizeof(gui_widget_t));
//     if(node)
//     {
//         gui_widget_t* widget = node->data;
//         widget->type = GUI_TYPE_STRING;
//         widget->color = color;
//         gui_SizeInit(widget, x, y, length, 0, (int)str);
//         insert_node(canvas->list, node, -1);
//         return widget;
//     }
//     return NULL;
// }

// void gui_HideCanvas(gui_canvas_t* canvas, int hide)
// {
//     if(hide)
//         canvas->visibility = gui_invisible;
//     else
//         canvas->visibility = gui_visible;
// }

// void gui_SetVisibility(gui_widget_t* widget, gui_visibility_t visibility)
// {
//     widget->status |= visibility;
// }

// void gui_SetFillstyle(gui_widget_t* widget, glib_fillstyle_t fillstyle)
// {
//     widget->status |= fillstyle << 1;
// }

// void gui_SetDirection(gui_widget_t* widget, glib_dir_t dir)
// {
//     widget->status |= dir << 2;
// }

// void gui_SetAlignment(gui_widget_t* widget, glib_align_t align)
// {
//     widget->status |= align << 4;
// }

// void gui_PaintCanvas(glib_painter_t* painter, gui_canvas_t* canvas)
// {
//     node_t* temp = canvas->list->head;
//     gui_widget_t* widget;
//     while(temp)
//     {
//         widget = temp->data;
//         temp = temp->next;
//         gui_size_t size = widget->size;
//         if((widget->status & GUI_STATUS_VISIBILITY_MASK) == gui_visible)
//         {
//             glib_fillstyle_t fillstyle = (glib_fillstyle_t)((widget->status & GUI_STATUS_FILLSTYLE_MASK) >> 1);
//             glib_dir_t direction = (glib_dir_t)((widget->status & GUI_STATUS_DIRECTION_MASK) >> 2);
//             glib_align_t alignment = (glib_align_t)((widget->status & GUI_STATUS_ALIGNMENT_MASK) >> 4);
            
//             if(alignment && widget->type != GUI_TYPE_STRING && widget->type != GUI_TYPE_NUM && widget->type != GUI_TYPE_CIRCLE && widget->type != GUI_TYPE_ELLIPSE)
//             {
//                 glib_Align(&size.x, &size.y, size.width, size.height, alignment);
//             }
            
//             switch(widget->type)
//             {
//                 case GUI_TYPE_ROUNDRECT:glib_DrawRoundRect(painter, size.x, size.y, size.width, size.height, size.r, fillstyle, widget->color);             break;
//                 case GUI_TYPE_STRING:   glib_ShowString(painter, size.x, size.y, (char*)size.r, size.width, alignment, direction, widget->color);           break;
//                 case GUI_TYPE_RECT:     glib_DrawRect(painter, size.x, size.y, size.width, size.height, fillstyle, widget->color);                          break;
//                 case GUI_TYPE_IMAGE:    glib_ShowMonoImage(painter, size.x, size.y, size.width, size.height, (uint8_t*)size.r, direction, widget->color);   break;
//                 case GUI_TYPE_NUM:                                                                                                                          break;
//                 case GUI_TYPE_HLINE:    glib_DrawHLine(painter, size.x, size.y, size.width, widget->color);                                                 break;
//                 case GUI_TYPE_VLINE:    glib_DrawVLine(painter, size.x, size.y, size.width, widget->color);                                                 break;
//                 case GUI_TYPE_CIRCLE:   glib_DrawCircle(painter, size.x, size.y, size.r, fillstyle, widget->color);                                         break;
//                 case GUI_TYPE_LINE:     glib_DrawLine(painter, size.x, size.y, size.width, size.height, widget->color);                                     break;
//                 case GUI_TYPE_ELLIPSE:  glib_DrawEllipse(painter, size.x, size.y, size.width, size.height, fillstyle, widget->color);                       break;
//                 default:break;
//             }
//         }
//     }
// }
