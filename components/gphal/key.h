#ifndef __GPHAL_KEY_H
#define __GPHAL_KEY_H

#include "gphal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    KEY_EVENT_PRESS,
    KEY_EVENT_LONG_PRESS,
    KEY_EVENT_RELEASE,
    KEY_EVENT_MULTI_PRESS,
} gphal_key_event_t;

typedef enum
{
    KEY_STATE_UNPRESSED,
    KEY_STATE_PRESSED,
} gphal_key_state_t;
typedef struct
{
    gphal_cb_t cb;
    void *user_data;
    gphal_state_t state;
    uint32_t hold_ticks;
    uint32_t debounce_ticks;
    uint32_t multi_press_interval_ticks;
    uint32_t pressed_ticks;
    uint32_t multi_press_ticks;
    uint32_t multi_press_count;
    gphal_key_state_t key_state;
    int pin;
    int pressed_level;
} gphal_key_t, *gphal_key_handle_t;

typedef struct
{
    gphal_key_handle_t self;
    gphal_key_event_t event;
    void *user_data;
} gphal_key_cb_param_t;

typedef struct
{
    uint32_t hold_ticks;
    uint32_t debounce_ticks;
    int pin;
    int pressed_level;
    gphal_cb_t cb;
    void *user_data;
} gphal_key_init_t;

gphal_key_handle_t gphal_key_init(gphal_key_init_t *init);
void gphal_key_deinit(gphal_key_handle_t key);
void gphal_key_enable(gphal_key_handle_t key);
void gphal_key_disable(gphal_key_handle_t key);
void gphal_key_start(gphal_key_handle_t key);
void gphal_key_stop(gphal_key_handle_t key);
void gphal_key_handler(gphal_key_handle_t key);
void gphal_key_register_callback(gphal_key_handle_t key, gphal_cb_t cb, void *user_data);
void gphal_key_set_hold(gphal_key_handle_t key, uint32_t hold_ticks);
void gphal_key_set_debounce(gphal_key_handle_t key, uint32_t debounce_ticks);
void gphal_key_set_pressed_level(gphal_key_handle_t key, int pressed_level);
void gphal_key_set_pin(gphal_key_handle_t key, int pin);
void gphal_key_set_multi_press(gphal_key_handle_t key, uint32_t multi_press_interval_ticks);
extern inline uint32_t gphal_key_get_multi_press_count(gphal_key_handle_t key);

#ifdef __cplusplus
}
#endif

#endif