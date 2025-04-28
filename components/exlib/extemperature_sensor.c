#include "extemperature_sensor.h"

#if SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

static temperature_sensor_handle_t temp_sensor;

void extemperature_sensor_init(void)
{
    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));
}

void extemperature_sensor_enable(bool enable)
{
    if(enable)
        ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));
    else
        ESP_ERROR_CHECK(temperature_sensor_disable(temp_sensor));
}

float extemperature_sensor_read(void)
{
    float tsens_value = 0;
    ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));
    return tsens_value;
}

#endif