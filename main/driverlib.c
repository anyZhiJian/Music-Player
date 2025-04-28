#include <stdio.h>
#include "exlib.h"
#include "on_board.h"
#include "lvgl.h"
#include <dirent.h>
#include <sys/stat.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

const char *TAG = "main";

uint8_t audio_buffer[1152]; // PCM数据缓冲区
uint8_t header[44];
QueueHandle_t queue;
i2s_chan_handle_t tx_chan;
FILE *fp;
int volume = 50;
char list[200][256];
int cnt;
int focus;
int state;
int mode;
uint16_t databit;
uint32_t datasize;
uint32_t data_size_read;

void print_list(void)
{
    printf("----------------------------------music list----------------------------------\n");
    for (int i = 1; i <= cnt; i++)
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

void uart_task(void *p)
{
    char buffer[1024];
    while (1)
    {
        exuart_queue_handler(UART_NUM_0, queue, buffer, 0, portMAX_DELAY);
        switch (buffer[0])
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
            if (focus < cnt - 1)
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

void app_main(void)
{
    exgpio_init(EXGPIO_PIN_TO_MASK(16), GPIO_MODE_OUTPUT, 0, 0, 0);
    gpio_set_level(16, 1);
    queue = exuart_init(UART_NUM_0, 115200, 43, 44);
    xTaskCreate(uart_task, "", 4096, NULL, 1, NULL);
    while(1)
    {
        if(exsdspi_init(SCLK, MOSI, MISO, 8) == ESP_OK)
        break;
    }
    DIR *dir = opendir(MOUNT_POINT);
    if (dir == NULL)
    {
        printf("Failed to open directory: %s\n", MOUNT_POINT);
        return;
    }

    printf("Contents of %s:\n", MOUNT_POINT);
    printf("----------------------------\n");

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        // 获取文件/目录完整路径
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", MOUNT_POINT, entry->d_name);

        // 获取文件信息（类型、大小）
        struct stat stat_buf;
        if (stat(full_path, &stat_buf) == 0)
        {
            printf("%-20s [%s] [Size: %lld bytes]\n",
                   entry->d_name,
                   S_ISDIR(stat_buf.st_mode) ? "DIR " : "FILE",
                   (long long)stat_buf.st_size);
        }
        else
        {
            printf("%s\n", entry->d_name);
        }
        if (!S_ISDIR(stat_buf.st_mode))
        {
            sprintf(list[cnt], "%s", entry->d_name);
            cnt++;
        }
    }

    closedir(dir);
    printf("----------------------------\n");

    exi2s_new_channel(I2S_NUM_0, &tx_chan, NULL);
    exi2s_std_channel_init(tx_chan, -1, 4, 18, 7, -1);
    read_music(list[focus]);

    while (1)
    {
        size_t bytes_writen;
        ssize_t bytes_read;
        while (1)
        {
            bytes_writen = 0;
            bytes_read = 0;
            bytes_read = fread(audio_buffer, sizeof(uint8_t), sizeof(audio_buffer), fp);
            if (data_size_read < datasize)
            {
                if (volume)
                {
                    double volumeFactor = pow(10, (-50 + volume / 2) / 20.0);
                    switch (databit)
                    {
                    case 8:
                        for (int i = 0; i < bytes_read; i++)
                            ((int8_t *)audio_buffer)[i] *= volumeFactor;
                        break;
                    case 16:
                        for (int i = 0; i < bytes_read / 2; i++)
                            ((int16_t *)audio_buffer)[i] *= volumeFactor;
                        break;
                    case 32:
                        for (int i = 0; i < bytes_read / 4; i++)
                            ((int32_t *)audio_buffer)[i] *= volumeFactor;
                        break;
                    case 24:
                        for (int i = 0; i < bytes_read; i += 3)
                        {
                            int32_t sample = (int32_t)((audio_buffer[i + 2] << 16) | (audio_buffer[i + 1] << 8) | audio_buffer[i]);
                            sample = sample << 8;
                            sample = sample >> 8;
                            sample  *= volumeFactor;
                            audio_buffer[i] = sample;
                            audio_buffer[i + 1] = (sample >> 8);
                            audio_buffer[i + 2] = (sample >> 16);
                        }
                        break;
                    }
                }
                else
                    memset(audio_buffer, 0, bytes_read);
                i2s_channel_write(tx_chan, audio_buffer, bytes_read, &bytes_writen, portMAX_DELAY);
                data_size_read += bytes_read;
            }
            else
            {
                break;
            }
        }
        i2s_channel_disable(tx_chan);
        if (mode)
        {
            rewind(fp);
            fseek(fp, 44, SEEK_SET);
        }    
        else
        {
            focus++;
            focus %= cnt;
            read_music(list[focus]);
        }
        i2s_channel_enable(tx_chan);
    }
}