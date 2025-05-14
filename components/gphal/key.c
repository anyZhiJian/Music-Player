#include "key.h"

gphal_key_handle_t gphal_key_init(gphal_key_init_t *init)
{
    gphal_assert(init);
    gphal_key_handle_t key = NULL;
    key = (gphal_key_handle_t)gphal_malloc(sizeof(gphal_key_t));
    if(key)
    {
        key->cb = init->cb;
        key->user_data = init->user_data;
        key->state = STATE_INIT;
        key->hold_ticks = init->hold_ticks;
        key->debounce_ticks = init->debounce_ticks;
        key->multi_press_interval_ticks = 0;
        key->pressed_ticks = 0;
        key->multi_press_count = 0;
        key->multi_press_ticks = 0;
        key->key_state = KEY_STATE_UNPRESSED;
        key->pin = init->pin;
        key->pressed_level = init->pressed_level;
    }
    return key;
}

void gphal_key_deinit(gphal_key_handle_t key)
{
    gphal_assert(key);
    if(key->state == STATE_RUNNUNG) return;
    gphal_free(key);
}

void gphal_key_enable(gphal_key_handle_t key)
{
    gphal_assert(key);
    if(key->state != STATE_INIT || key->cb == NULL) return;
    key->pressed_ticks = 0;
    key->multi_press_count = 0;
    key->multi_press_ticks = 0;
    key->key_state = KEY_STATE_UNPRESSED;
    key->state = STATE_READY;
}

void gphal_key_disable(gphal_key_handle_t key)
{
    gphal_assert(key);
    if(key->state == STATE_INIT) return;
    key->state = STATE_INIT;
}

void gphal_key_start(gphal_key_handle_t key)
{
    gphal_assert(key);
    if(key->state != STATE_READY) return;
    key->state = STATE_RUNNUNG;
}

void gphal_key_stop(gphal_key_handle_t key)
{
    gphal_assert(key);
    if(key->state != STATE_RUNNUNG) return;
    key->state = STATE_READY;
}

void gphal_key_handler(gphal_key_handle_t key)
{
    gphal_assert(key);
    if(key->state != STATE_RUNNUNG) return;
    if(key->multi_press_count > 0)
    {
        key->multi_press_ticks++;
        if(key->multi_press_ticks > key->multi_press_interval_ticks)
        {
            key->multi_press_count = 0;
            key->multi_press_ticks = 0;
        }
    }
    if(key->key_state == KEY_STATE_UNPRESSED)
    {
        if(gphal_gpio_read(key->pin) == key->pressed_level)
        {
            if(key->pressed_ticks >= key->debounce_ticks)
            {
                key->key_state = KEY_STATE_PRESSED;
                key->pressed_ticks = 0;
                key->multi_press_count++;
                key->multi_press_ticks = 0;
                gphal_key_cb_param_t param;
                param.self = key;
                param.user_data = key->user_data;
                if(key->multi_press_count > 1)
                    param.event = KEY_EVENT_MULTI_PRESS;
                else
                    param.event = KEY_EVENT_PRESS;
                key->cb(&param);
            }
            key->pressed_ticks++;
        }
    }
    else if(key->key_state == KEY_STATE_PRESSED)
    {
        key->pressed_ticks++;
        gphal_key_cb_param_t param;
        param.self = key;
        param.user_data = key->user_data;
        if(gphal_gpio_read(key->pin) != key->pressed_level)
        {
            key->key_state = KEY_STATE_UNPRESSED;
            param.event = KEY_EVENT_RELEASE;
            key->cb(&param);
            key->pressed_ticks = 0;
        }
        else if(key->pressed_ticks == key->hold_ticks)
        {
            param.event = KEY_EVENT_LONG_PRESS;
            key->cb(&param);
        }
    }
}

void gphal_key_register_callback(gphal_key_handle_t key, gphal_cb_t cb, void *user_data)
{
    gphal_assert(key);
    if(key->state == STATE_RUNNUNG) return;
    key->cb = cb;
    key->user_data = user_data;
}

void gphal_key_set_hold(gphal_key_handle_t key, uint32_t hold_ticks)
{
    gphal_assert(key);
    if(key->state == STATE_RUNNUNG) return;
    key->hold_ticks = hold_ticks;
}

void gphal_key_set_debounce(gphal_key_handle_t key, uint32_t debounce_ticks)
{
    gphal_assert(key);
    if(key->state == STATE_RUNNUNG) return;
    key->debounce_ticks = debounce_ticks;
}

void gphal_key_set_pressed_level(gphal_key_handle_t key, int pressed_level)
{
    gphal_assert(key);
    if(key->state == STATE_RUNNUNG) return;
    key->pressed_level = pressed_level;
}

void gphal_key_set_pin(gphal_key_handle_t key, int pin)
{
    gphal_assert(key);
    if(key->state == STATE_RUNNUNG) return;
    key->pin = pin;
}

void gphal_key_set_multi_press(gphal_key_handle_t key, uint32_t multi_press_interval_ticks)
{
    gphal_assert(key);
    if(key->state == STATE_RUNNUNG) return;
    key->multi_press_interval_ticks = multi_press_interval_ticks;
}

inline uint32_t gphal_key_get_multi_press_count(gphal_key_handle_t key)
{
    gphal_assert(key);
    return key->multi_press_count;
}