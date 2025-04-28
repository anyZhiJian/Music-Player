#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

#include "driver/temperature_sensor.h"

void extemperature_sensor_init(void);
void extemperature_sensor_enable(bool enable);
float extemperature_sensor_read(void);

#endif

#ifdef __cplusplus
}
#endif