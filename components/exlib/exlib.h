#pragma once

#define INCLUDE_EXLIB_ADC                   1
#define INCLUDE_EXLIB_ESP_TIMER             1
#define INCLUDE_EXLIB_GPIO                  1
#define INCLUDE_EXLIB_GPTIMER               1
#define INCLUDE_EXLIB_I2C                   1
#define INCLUDE_EXLIB_I2S                   1
#define INCLUDE_EXLIB_LEDC                  1
#define INCLUDE_EXLIB_MCPWM                 0
#define INCLUDE_EXLIB_PCNT                  1
#define INCLUDE_EXLIB_RMT                   1
#define INCLUDE_EXLIB_SD_CARD               1
#define INCLUDE_EXLIB_SDM                   1
#define INCLUDE_EXLIB_SOCKET                1
#define INCLUDE_EXLIB_SPI                   1
#define INCLUDE_EXLIB_TEMPERATURE_SENSOR    1
#define INCLUDE_EXLIB_TOUCH_SENSOR          0
#define INCLUDE_EXLIB_TWAI                  0
#define INCLUDE_EXLIB_UART                  1
#define INCLUDE_EXLIB_USB                   0
#define INCLUDE_EXLIB_WIFI                  1

#if INCLUDE_EXLIB_ADC
#include "exadc.h"
#endif
#if INCLUDE_EXLIB_ESP_TIMER
#include "exesp_timer.h"
#endif
#if INCLUDE_EXLIB_GPIO
#include "exgpio.h"
#endif
#if INCLUDE_EXLIB_GPTIMER
#include "exgptimer.h"
#endif
#if INCLUDE_EXLIB_I2C
#include "exi2c.h"
#endif
#if INCLUDE_EXLIB_I2S
#include "exi2s.h"
#endif
#if INCLUDE_EXLIB_LEDC
#include "exledc.h"
#endif
#if INCLUDE_EXLIB_MCPWM
#include "exmcpwm.h"
#endif
#if INCLUDE_EXLIB_PCNT
#include "expcnt.h"
#endif
#if INCLUDE_EXLIB_RMT
#include "exrmt.h"
#endif
#if INCLUDE_EXLIB_SD_CARD
#include "exsd_card.h"
#endif
#if INCLUDE_EXLIB_SDM
#include "exsdm.h"
#endif
#if INCLUDE_EXLIB_SOCKET
#include "exsocket.h"
#endif
#if INCLUDE_EXLIB_SPI
#include "exspi.h"
#endif
#if INCLUDE_EXLIB_TEMPERATURE_SENSOR
#include "extemperature_sensor.h"
#endif
#if INCLUDE_EXLIB_TOUCH_SENSOR
#include "extouch_sensor.h"
#endif
#if INCLUDE_EXLIB_TWAI
#include "extwai.h"
#endif
#if INCLUDE_EXLIB_UART
#include "exuart.h"
#endif
#if INCLUDE_EXLIB_USB
#include "exusb.h"
#endif
#if INCLUDE_EXLIB_WIFI
#include "exwifi.h"
#endif