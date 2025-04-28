#include "font.h"
#include "font_Consolas.h"
#include "font_Song_Typeface.h"
// #include "font_xxx.h"(自定义字体添加字段)

fontinfo_t font_get_font_info(font_t font)
{
    fontinfo_t fontinfo = {0};
    switch(font)
    {
        case font_Song_Typeface_6x12:   fontinfo.width = 6,     fontinfo.height = 12,   fontinfo.font = (uint8_t*)Song_Typeface_6x12;   break;
        case font_Song_Typeface_8x16:   fontinfo.width = 8,     fontinfo.height = 16,   fontinfo.font = (uint8_t*)Song_Typeface_8x16;   break;
        case font_Song_Typeface_10x20:  fontinfo.width = 10,    fontinfo.height = 20,   fontinfo.font = (uint8_t*)Song_Typeface_10x20;  break;
        case font_Song_Typeface_12x24:  fontinfo.width = 12,    fontinfo.height = 24,   fontinfo.font = (uint8_t*)Song_Typeface_12x24;  break;
        case font_Song_Typeface_14x28:  fontinfo.width = 14,    fontinfo.height = 28,   fontinfo.font = (uint8_t*)Song_Typeface_14x28;  break;
        case font_Song_Typeface_16x32:  fontinfo.width = 16,    fontinfo.height = 32,   fontinfo.font = (uint8_t*)Song_Typeface_16x32;  break;
        case font_Consolas_6x12:        fontinfo.width = 6,     fontinfo.height = 12,   fontinfo.font = (uint8_t*)Consolas_6x12;        break;
        case font_Consolas_8x16:        fontinfo.width = 8,     fontinfo.height = 16,   fontinfo.font = (uint8_t*)Consolas_8x16;        break;
        case font_Consolas_10x20:       fontinfo.width = 10,    fontinfo.height = 20,   fontinfo.font = (uint8_t*)Consolas_10x20;       break;
        case font_Consolas_12x24:       fontinfo.width = 12,    fontinfo.height = 24,   fontinfo.font = (uint8_t*)Consolas_12x24;       break;
        case font_Consolas_14x28:       fontinfo.width = 14,    fontinfo.height = 28,   fontinfo.font = (uint8_t*)Consolas_14x28;       break;
        case font_Consolas_16x32:       fontinfo.width = 16,    fontinfo.height = 32,   fontinfo.font = (uint8_t*)Consolas_16x32;       break;
        // case font_xxx:fontinfo.width = xx,fontinfo.height = xx,fontinfo.font = (uint8_t*)xxx;break;(自定义字体添加字段)
    }
    fontinfo.fontsize = fontinfo.width * ((fontinfo.height + 8 - 1) / 8); // 适用于非负数的向上取整除法(a + b - 1) / b
    return fontinfo;
}
