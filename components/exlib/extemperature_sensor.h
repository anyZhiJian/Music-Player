#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/temperature_sensor.h"

void extemperature_sensor_init(void);
void extemperature_sensor_enable(bool enable);
float extemperature_sensor_read(void);

#ifdef __cplusplus
}
#endif