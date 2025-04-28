#include "exgptimer.h"

#define TIMER_DIRECTION GPTIMER_COUNT_UP

gptimer_handle_t exgptimer_init(uint32_t resolution_hz, uint64_t reload_count, uint64_t alarm_count ,bool auto_reload)
{
    gptimer_handle_t gptimer_handle = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = TIMER_DIRECTION,
        .resolution_hz = resolution_hz,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer_handle));
    gptimer_alarm_config_t alarm_config = {
        .reload_count = reload_count,
        .alarm_count = alarm_count,
        .flags.auto_reload_on_alarm = auto_reload,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer_handle, &alarm_config));
    ESP_ERROR_CHECK(gptimer_set_raw_count(gptimer_handle, 0));
    return gptimer_handle;
}

/**
  * @note a timer must be in the "init" state before it can be deleted.
  */
void exgptimer_deinit(gptimer_handle_t gptimer_handle)
{
    ESP_ERROR_CHECK(gptimer_del_timer(gptimer_handle));
}

/**
  * @note before updating the alarm callback, we should make sure the timer is not in the enable state.
  */
void exgptimer_set_cbs(gptimer_handle_t gptimer_handle, gptimer_alarm_cb_t on_alarm, void *user_data)
{
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_alarm,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer_handle, &cbs, user_data));
}

void exgptimer_enable_and_start(gptimer_handle_t gptimer_handle)
{
    ESP_ERROR_CHECK(gptimer_enable(gptimer_handle));
    ESP_ERROR_CHECK(gptimer_start(gptimer_handle));
}

void exgptimer_stop_and_disable(gptimer_handle_t gptimer_handle)
{
    ESP_ERROR_CHECK(gptimer_stop(gptimer_handle));
    ESP_ERROR_CHECK(gptimer_disable(gptimer_handle));
}