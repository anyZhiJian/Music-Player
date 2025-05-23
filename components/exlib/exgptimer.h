#pragma once

#include "driver/gptimer.h"

#ifdef __cplusplus
extern "C" {
#endif

gptimer_handle_t exgptimer_init(uint32_t resolution_hz, uint64_t reload_count, uint64_t alarm_count ,bool auto_reload);
void exgptimer_deinit(gptimer_handle_t gptimer);
void exgptimer_register_cbs(gptimer_handle_t gptimer, gptimer_alarm_cb_t on_alarm, void *user_data);
void exgptimer_enable_and_start(gptimer_handle_t gptimer);
void exgptimer_stop_and_disable(gptimer_handle_t gptimer);

#ifdef __cplusplus
}
#endif