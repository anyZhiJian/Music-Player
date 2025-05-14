#include "timer.h"

gphal_timer_handle_t gphal_timer_init(uint32_t period_ticks, gphal_cb_t cb, void *user_data)
{
    gphal_timer_handle_t timer = NULL;
    if(!period_ticks || period_ticks > ((uint32_t)-1) / 2 + 1) return timer;
    timer = (gphal_timer_handle_t)gphal_malloc(sizeof(gphal_timer_t));
    if(timer)
    {
        timer->cb = cb;
        timer->user_data = user_data;
        timer->state = STATE_INIT;
        timer->period_ticks = period_ticks;
        timer->target_ticks = 0;
        timer->remain_ticks = 0;
        timer->alarm_count = 0;
    }
    return timer;
}

void gphal_timer_deinit(gphal_timer_handle_t timer)
{
    gphal_assert(timer);
    if(timer->state == STATE_RUNNUNG) return;
    gphal_free(timer);
}

void gphal_timer_enable(gphal_timer_handle_t timer)
{
    gphal_assert(timer);
    if(timer->state != STATE_INIT || timer->cb == NULL) return;
    timer->remain_ticks = timer->period_ticks;
    timer->state = STATE_READY;
}

void gphal_timer_disable(gphal_timer_handle_t timer)
{
    gphal_assert(timer);
    if(timer->state == STATE_RUNNUNG) return;
    timer->state = STATE_INIT;
    timer->alarm_count = 0;
}

void gphal_timer_start(gphal_timer_handle_t timer)
{
    gphal_assert(timer);
    if(timer->state != STATE_READY) return;
    timer->target_ticks = gphal_get_ticks() + timer->remain_ticks;
    timer->state = STATE_RUNNUNG;
}

void gphal_timer_stop(gphal_timer_handle_t timer)
{
    gphal_assert(timer);
    if(timer->state != STATE_RUNNUNG) return;
    timer->state = STATE_READY;
}

void gphal_timer_handler(gphal_timer_handle_t timer)
{
    gphal_assert(timer);
    if(timer->state != STATE_RUNNUNG) return;
    uint32_t now_ticks = gphal_get_ticks();
    timer->remain_ticks = timer->target_ticks - now_ticks;
    if((int32_t)(timer->remain_ticks) <= 0)
    {
        timer->target_ticks = now_ticks + timer->period_ticks;
        timer->alarm_count++;
        gphal_timer_cb_param_t param;
        param.self = timer;
        param.user_data = timer->user_data;
        timer->cb(&param);
    }
}

void gphal_timer_register_callback(gphal_timer_handle_t timer, gphal_cb_t cb, void *user_data)
{
    gphal_assert(timer);
    if(timer->state == STATE_RUNNUNG) return;
    timer->cb = cb;
    timer->user_data = user_data;
}

void gphal_timer_set_period(gphal_timer_handle_t timer, uint32_t period_ticks)
{
    gphal_assert(timer);
    if(timer->state == STATE_RUNNUNG) return;
    timer->period_ticks = period_ticks;
}

inline uint32_t gphal_timer_get_alarm_count(gphal_timer_handle_t timer)
{
    gphal_assert(timer);
    return timer->alarm_count;
}

inline void gphal_timer_clear_alarm_count(gphal_timer_handle_t timer)
{
    gphal_assert(timer);
    timer->alarm_count = 0;
}