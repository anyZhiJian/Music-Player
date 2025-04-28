#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/sdm.h"

sdm_channel_handle_t exsdm_init(int gpio_num, uint32_t rate_hz);
void exsdm_deinit(sdm_channel_handle_t channel);
void exsdm_enalbe(sdm_channel_handle_t channel, int enable);
void exsdm_set_pulse_density(sdm_channel_handle_t sdm_chan, float vout_percent);

#ifdef __cplusplus
}
#endif