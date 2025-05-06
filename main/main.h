#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "esp_log.h"
#include "lvgl.h"

#include "exlib.h"
#include "on_board.h"

#include <dirent.h>

#include "key.h"
#include "esp_heap_caps.h"
#include "esp_freertos_hooks.h"
const char *TAG = "main";
void lvgl_draw(void);

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
uint8_t audio_buffer[1152]; // PCM数据缓冲区
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

void lvgl_draw(void)
{
    
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
        beep = 1;
        key_mode = !key_mode;
        if(key_mode)
        ESP_LOGI(TAG, "滚动滚轮调节音量");
        else
        ESP_LOGI(TAG, "滚动滚轮切换歌曲");
        break;
        case KEY_EVENT_MULTI_PRESS:
        if(gphal_key_get_multi_press_count(data->self) == 2)
            print_list();
        break;
        default:break;
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
    extemperature_sensor_enable(true);
    int iram;
    int psram;
    int dmaram;
    while(1)
    {
        iram = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
        dmaram = heap_caps_get_free_size(MALLOC_CAP_DMA);
        ESP_LOGI(TAG, "CPU temprature:%.1f C", extemperature_sensor_read());
        ESP_LOGI(TAG, "IRAM:%d bytes(%.1f KB)", iram, iram / 1024.0);
        ESP_LOGI(TAG, "PSRAM:%d bytes(%.1f KB)", psram, psram / 1024.0);
        ESP_LOGI(TAG, "DMARAM:%d bytes(%.1f KB)", dmaram, dmaram / 1024.0);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void buzzer_task(void *param)
{
    // exledc_timer_init(LEDC_TIMER_1, 1000);
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