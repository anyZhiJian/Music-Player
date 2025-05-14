#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "esp_log.h"

#include "exlib.h"
#include "on_board.h"

#include <dirent.h>

#include "key.h"
#include "esp_heap_caps.h"
#include "esp_freertos_hooks.h"
const char *TAG = "main";
void lvgl_draw(void);
float cpu_temp;
lv_group_t* group;
uint8_t uart_rbuffer[1024];
uint8_t uart_wbuffer[1024];
void uart_task(void *param);
void lvgl_task(void *param);
void encoder_task(void *param);
void key_task(void *param);
void led_task(void *param);
void temp_task(void *param);
void buzzer_task(void *param);
//////////////////////////////////////////////////////////////////////////
uint8_t audio_buffer[768]; // PCM数据缓冲区
uint8_t header[44];
QueueHandle_t queue;
i2s_chan_handle_t tx_chan;
FILE *fp;
int volume = 50;
char list[200][256];
int music_cnt;
int focus;
int state;
int mode;
uint16_t databit;
uint32_t datasize;
uint32_t data_size_read;
int key_mode;
int beep;

void print_list(void)
{
    printf("----------------------------------music list----------------------------------\n");
    for (int i = 1; i <= music_cnt; i++)
    {
        printf("%d\t%s\n", i, list[i - 1]);
    }
    printf("------------------------------------------------------------------------------\n");
}

void read_music(const char *music)
{
    if (state == 1)
    {
        fclose(fp);
    }
    memset(audio_buffer, 0, sizeof(audio_buffer)); // 清空缓冲区可以很好的消除切歌时的杂音
    char s[256];
    sprintf(s, "%s/%s", MOUNT_POINT, music);
    fp = fopen(s, "rb");
    fread(&header, sizeof(uint8_t), 44, fp);
    uint32_t sample_rate = *(uint32_t *)(header + 24); // 采样率
    uint16_t channal_num = *(uint16_t *)(header + 22); // 声道数
    uint16_t bit_width = *(uint32_t *)(header + 34);   // 位深度
    uint32_t data_size = *(uint32_t *)(header + 40);   // 数据大小
    ESP_LOGI(TAG, "sample_rate:%d", (int)sample_rate);
    ESP_LOGI(TAG, "channal_num:%d", (int)channal_num);
    ESP_LOGI(TAG, "bit_width:%d", (int)bit_width);
    ESP_LOGI(TAG, "data_size:%d", (int)data_size);
    exi2s_std_reconfig(tx_chan, sample_rate, channal_num, bit_width);
    state = 1;
    ESP_LOGI(TAG, "当前曲目：%s", music);
    datasize = data_size;
    databit = bit_width;
    data_size_read = 0;
}
//////////////////////////////////////////////////////////////////////////
void init(void)
{
    xTaskCreate(uart_task, "", 4096, "", 1, NULL);
    xTaskCreate(lvgl_task, "", 4096, "", 1, NULL);
    xTaskCreate(encoder_task, "", 4096, "", 1, NULL);
    xTaskCreate(key_task, "", 4096, "", 1, NULL);
    xTaskCreate(led_task, "", 4096, "", 1, NULL);
    xTaskCreate(temp_task, "", 4096, "", 1, NULL);
    xTaskCreate(buzzer_task, "", 4096, "", 1, NULL);
}

void uart_task(void *param)
{
    exuart_init(UART_NUM_0, 115200, 43, 44);
    while(1)
    {
        exuart_read(UART_NUM_0, uart_rbuffer, 1, portMAX_DELAY);
        switch (uart_rbuffer[0])
        {
        case '0':
            i2s_channel_disable(tx_chan);
            ESP_LOGI(TAG, "音乐暂停");
            break;
        case '1':
            i2s_channel_enable(tx_chan);
            ESP_LOGI(TAG, "开始播放");
            break;
        case '8':
            if (volume + 10 <= 100)
                volume += 10;
            ESP_LOGI(TAG, "当前音量：%d", volume);
            break;
        case '2':
            if (volume - 10 >= 0)
                volume -= 10;
            ESP_LOGI(TAG, "当前音量：%d", volume);
            break;
        case '4':
            ESP_LOGI(TAG, "播放上一首");
            i2s_channel_disable(tx_chan);
            if (focus > 0)
                focus--;
            read_music(list[focus]);
            i2s_channel_enable(tx_chan);
            break;
        case '6':
            ESP_LOGI(TAG, "播放下一首");
            i2s_channel_disable(tx_chan);
            if (focus < music_cnt - 1)
                focus++;
            read_music(list[focus]);
            i2s_channel_enable(tx_chan);
            break;
        case '7':
            ESP_LOGI(TAG, "播放模式已设置为单曲循环");
            mode = 1;
            break;
        case '9':
            ESP_LOGI(TAG, "播放模式已设置为顺序播放");
            mode = 0;
            break;
        case 'l':
            print_list();
            break;
        default:
            break;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////////
static void mask_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    static int16_t mask_top_id = -1;
    static int16_t mask_bottom_id = -1;

    if (code == LV_EVENT_COVER_CHECK) {
        lv_event_set_cover_res(e, LV_COVER_RES_MASKED);

    } else if (code == LV_EVENT_DRAW_MAIN_BEGIN) {
        /* add mask */
        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
        lv_coord_t font_h = lv_font_get_line_height(font);

        lv_area_t roller_coords;
        lv_obj_get_coords(obj, &roller_coords);

        lv_area_t rect_area;
        rect_area.x1 = roller_coords.x1;
        rect_area.x2 = roller_coords.x2;
        rect_area.y1 = roller_coords.y1;
        rect_area.y2 = roller_coords.y1 + (lv_obj_get_height(obj) - font_h - line_space) / 2;

        lv_draw_mask_fade_param_t * fade_mask_top = lv_mem_buf_get(sizeof(lv_draw_mask_fade_param_t));
        lv_draw_mask_fade_init(fade_mask_top, &rect_area, LV_OPA_TRANSP, rect_area.y1, LV_OPA_COVER, rect_area.y2);
        mask_top_id = lv_draw_mask_add(fade_mask_top, NULL);

        rect_area.y1 = rect_area.y2 + font_h + line_space - 1;
        rect_area.y2 = roller_coords.y2;

        lv_draw_mask_fade_param_t * fade_mask_bottom =lv_mem_buf_get(sizeof(lv_draw_mask_fade_param_t));
        lv_draw_mask_fade_init(fade_mask_bottom, &rect_area, LV_OPA_COVER, rect_area.y1, LV_OPA_TRANSP, rect_area.y2);
        mask_bottom_id = lv_draw_mask_add(fade_mask_bottom, NULL);

    } else if (code == LV_EVENT_DRAW_POST_END) {
        lv_draw_mask_fade_param_t * fade_mask_top = lv_draw_mask_remove_id(mask_top_id);
        lv_draw_mask_fade_param_t * fade_mask_bottom = lv_draw_mask_remove_id(mask_bottom_id);
        lv_draw_mask_free_param(fade_mask_top);
        lv_draw_mask_free_param(fade_mask_bottom);
        lv_mem_buf_release(fade_mask_top);
        lv_mem_buf_release(fade_mask_bottom);
        mask_top_id = -1;
        mask_bottom_id = -1;
    }
}
////////////////////////////////////////////////////////////////////////////////////////
void lvgl_draw(void)
{
    // LV_FONT_DECLARE(siyuan_12);
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_bg_color(&style, lv_color_black());
    lv_style_set_text_color(&style, lv_color_white());
    lv_style_set_border_width(&style, 0);
    lv_style_set_pad_all(&style, 0);
    lv_obj_add_style(lv_scr_act(), &style, 0);

    lv_obj_t *roller1 = lv_roller_create(lv_scr_act());
    lv_obj_add_style(roller1, &style, 0);
    lv_obj_set_style_bg_opa(roller1, LV_OPA_TRANSP, LV_PART_SELECTED);
    lv_obj_set_style_text_font(roller1, &lv_font_montserrat_22, LV_PART_SELECTED);

    lv_roller_set_options(roller1,
                        "January\n"
                        "February\n"
                        "March\n"
                        "April\n"
                        "May\n"
                        "June\n"
                        "July\n"
                        "August\n"
                        "September\n"
                        "October\n"
                        "November\n"
                        "December",
                        LV_ROLLER_MODE_NORMAL);

    lv_obj_center(roller1);
    lv_roller_set_visible_row_count(roller1, 3);
    lv_obj_add_event_cb(roller1, mask_event_cb, LV_EVENT_ALL, NULL);
    lv_group_add_obj(group, roller1);
}

void lvgl_task(void *param)
{
    int pin;
    #if !(TLCD)
    pin = 7;
    #else
    pin = 10;
    #endif
    exgpio_init(EXGPIO_PIN_MASK(pin), GPIO_MODE_OUTPUT, 0, 0, 0);
    gpio_set_level(pin, 1);
    lcd_init();
    exledc_timer_init(LEDC_TIMER_0, 1500);
    exledc_channel_config(LEDC_TIMER_0, LEDC_CHANNEL_0, 9);
    exledc_set_duty(LEDC_CHANNEL_0, 100);
    group = lv_group_create();
    lv_indev_set_group(lv_port_get_dev(), group); 
    lvgl_draw();
    TickType_t ticks = 0;
    while(1)
    {
        lv_task_handler();
        xTaskDelayUntil(&ticks, pdMS_TO_TICKS(20));
    }
}

void encoder_task(void *param)
{
    pcnt_unit_handle_t encoder = expcnt_encoder_init(5, 6);
    int cnt = 0;
    int ret = 0;
    while(1)
    {
        ret = expcnt_get_encoder_state(encoder, &cnt, 1);
        lcd_set_encoder_cnt(cnt);
        if(key_mode)
        {
            if(ret > 0)
            {
                beep = 1;
                if (volume + 10 <= 100)
                volume += 10;
                ESP_LOGI(TAG, "当前音量：%d", volume);
            }
            else if(ret < 0)
            {
                beep = 1;
                if (volume - 10 >= 0)
                volume -= 10;
                ESP_LOGI(TAG, "当前音量：%d", volume);
            }
        }
        else
        {
            if(ret > 0)
            {
                beep = 1;
                ESP_LOGI(TAG, "播放上一首");
                i2s_channel_disable(tx_chan);
                if (focus > 0)
                    focus--;
                read_music(list[focus]);
                i2s_channel_enable(tx_chan);
            }
            else if(ret < 0)
            {
                beep = 1;
                ESP_LOGI(TAG, "播放下一首");
                i2s_channel_disable(tx_chan);
                if (focus < music_cnt - 1)
                    focus++;
                read_music(list[focus]);
                i2s_channel_enable(tx_chan);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void key_cb(void *param)
{
    gphal_key_cb_param_t* data = (gphal_key_cb_param_t*)param;
    static int flag = 0;
    switch(data->event)
    {
        case KEY_EVENT_PRESS:
        lcd_set_encoder_key(1);
        flag = !flag;
        if(flag)
        {
            beep = 1;
            i2s_channel_enable(tx_chan);
            ESP_LOGI(TAG, "开始播放");
        }
        else
        {
            beep = 1;
            i2s_channel_disable(tx_chan);
            ESP_LOGI(TAG, "音乐暂停");
        }
        break;
        case KEY_EVENT_LONG_PRESS:
        lcd_set_encoder_key(1);
        beep = 1;
        key_mode = !key_mode;
        if(key_mode)
        ESP_LOGI(TAG, "滚动滚轮调节音量");
        else
        ESP_LOGI(TAG, "滚动滚轮切换歌曲");
        break;
        case KEY_EVENT_MULTI_PRESS:
        lcd_set_encoder_key(1);
        if(gphal_key_get_multi_press_count(data->self) == 2)
            print_list();
        break;
        default:lcd_set_encoder_key(0);break;
    }
}

void key_task(void *param)
{
    exgpio_init(EXGPIO_PIN_MASK(4), GPIO_MODE_INPUT, 1, 0, 0);
    gphal_key_init_t key_config = {
        .cb = key_cb,
        .debounce_ticks = 1,
        .hold_ticks = 50,
        .pin = 4,
        .pressed_level = 0,
    };
    gphal_key_handle_t key = gphal_key_init(&key_config);
    gphal_key_set_multi_press(key, 15);
    gphal_key_enable(key);
    gphal_key_start(key);
    TickType_t ticks = 0;
    while(1)
    {
        gphal_key_handler(key);
        xTaskDelayUntil(&ticks, pdMS_TO_TICKS(20));
    }
}

void led_task(void *param)
{
    ws2812_handle_t led = ws2812_init(38, 1);
    int i = 0;
    while(1)
    {
        ws2812_set_hsv_all(led, i, 1, 1);
        ws2812_update_color(led);
        i++;
        i%=361;
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

void temp_task(void *param)
{
    extemperature_sensor_init();
    extemperature_sensor_enable();
    int iram;
    int psram;
    int dmaram;
    while(1)
    {
        cpu_temp = extemperature_sensor_read();
        iram = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
        dmaram = heap_caps_get_free_size(MALLOC_CAP_DMA);
        ESP_LOGI(TAG, "CPU temprature:%.1f C", cpu_temp);
        ESP_LOGI(TAG, "IRAM:%d bytes(%.1f KB)", iram, iram / 1024.0);
        ESP_LOGI(TAG, "PSRAM:%d bytes(%.1f KB)", psram, psram / 1024.0);
        ESP_LOGI(TAG, "DMARAM:%d bytes(%.1f KB)", dmaram, dmaram / 1024.0);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void buzzer_task(void *param)
{
    exledc_channel_config(LEDC_TIMER_0, LEDC_CHANNEL_1, 18);
    while(1)
    {
        if(beep)
        {
            beep = 0;
            exledc_set_duty(LEDC_CHANNEL_1, 50);
            vTaskDelay(pdMS_TO_TICKS(50));
            exledc_set_duty(LEDC_CHANNEL_1, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}