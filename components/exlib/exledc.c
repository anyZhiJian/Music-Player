#include "exledc.h"

void exledc_timer_init(ledc_timer_t timer, uint32_t freq_hz)
{
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_10_BIT,
        .timer_num        = timer,
        .freq_hz          = freq_hz,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
}

void exledc_channel_config(ledc_timer_t timer, ledc_channel_t channel, int gpio_num)
{
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = channel,
        .timer_sel      = timer,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = gpio_num,
        .duty           = 0,
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void exledc_set_duty(ledc_channel_t channel, uint32_t duty)
{
    ESP_ERROR_CHECK(ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, channel, duty, 0));
}

void exledc_fade_func_install(void)
{
    ESP_ERROR_CHECK(ledc_fade_func_install(0));
}

void exledc_fade_with_time(ledc_channel_t channel, uint32_t tar_duty, uint32_t ms, ledc_fade_mode_t fade_mode)
{
    ESP_ERROR_CHECK(ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, channel, tar_duty, ms, fade_mode));
}

void exledc_fade_with_step(ledc_channel_t channel, uint32_t tar_duty, int step_scale, int cycle_num, ledc_fade_mode_t fade_mode)
{
    ESP_ERROR_CHECK(ledc_set_fade_step_and_start(LEDC_LOW_SPEED_MODE, channel, tar_duty, step_scale, cycle_num, fade_mode));
}