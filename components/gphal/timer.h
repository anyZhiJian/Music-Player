#ifndef __GPHAL_TIMER_H
#define __GPHAL_TIMER_H

#include "gphal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    gphal_cb_t cb;
    void *user_data;
    gphal_state_t state;
    uint32_t period_ticks;
    uint32_t target_ticks;
    uint32_t remain_ticks;
    uint32_t alarm_count;
} gphal_timer_t, *gphal_timer_handle_t;

typedef struct
{
    gphal_timer_handle_t self;
    void *user_data;
} gphal_timer_cb_param_t;

gphal_timer_handle_t gphal_timer_init(uint32_t period, gphal_cb_t cb, void *user_data);
void gphal_timer_deinit(gphal_timer_handle_t timer);
void gphal_timer_enable(gphal_timer_handle_t timer);
void gphal_timer_disable(gphal_timer_handle_t timer);
void gphal_timer_start(gphal_timer_handle_t timer);
void gphal_timer_stop(gphal_timer_handle_t timer);
void gphal_timer_handler(gphal_timer_handle_t timer);
void gphal_timer_register_callback(gphal_timer_handle_t timer, gphal_cb_t cb, void *user_data);
void gphal_timer_set_period(gphal_timer_handle_t timer, uint32_t period_ticks);
extern inline uint32_t gphal_timer_get_alarm_count(gphal_timer_handle_t timer);
extern inline void gphal_timer_clear_alarm_count(gphal_timer_handle_t timer);

#ifdef __cplusplus
}
#endif

#endif