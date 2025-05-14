#include "main.h"

void app_main(void)
{
    // exwifi_init_apsta_mode("testwifi", "testwifi", "music player", "esp32-s3");
    // exwifi_napt();// BUG：开启网络后 SD card 可能挂载不上
    init();
    exgpio_init(EXGPIO_PIN_MASK(39), GPIO_MODE_OUTPUT, 0, 0, 0);
    exgpio_init(EXGPIO_PIN_MASK(40), GPIO_MODE_OUTPUT, 0, 0, 0);
    exgpio_init(EXGPIO_PIN_MASK(41), GPIO_MODE_OUTPUT, 0, 0, 0);
    exgpio_init(EXGPIO_PIN_MASK(42), GPIO_MODE_OUTPUT, 0, 0, 0);
    exgpio_init(EXGPIO_PIN_MASK(7), GPIO_MODE_OUTPUT, 0, 0, 0);
    gpio_set_level(39, 0);
    gpio_set_level(40, 0);
    gpio_set_level(41, 1);
    gpio_set_level(42, 0);
    gpio_set_level(7, 0);
    while(1)
    {
        if(exsd_card_sdspi_init(SPI2_HOST, 8) == ESP_OK)
            break;
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    DIR *dir = opendir(MOUNT_POINT);
    if (dir == NULL)
    {
        printf("Failed to open directory: %s\n", MOUNT_POINT);
        return;
    }

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
        if (!S_ISDIR(stat_buf.st_mode))
        {
            sprintf(list[music_cnt], "%s", entry->d_name);
            music_cnt++;
        }
    }
    closedir(dir);
    
    exi2s_new_channel(I2S_NUM_0, &tx_chan, NULL);
    exi2s_std_channel_init(tx_chan, -1, 15, 17, 16, -1);
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
        memset(audio_buffer, 0, sizeof(audio_buffer));
        i2s_channel_disable(tx_chan);
        if (!mode)
        {
            focus++;
            focus %= music_cnt;
        }
        read_music(list[focus]);
        i2s_channel_enable(tx_chan);
    }
}