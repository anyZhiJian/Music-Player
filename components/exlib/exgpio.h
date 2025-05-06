#pragma once

#include "driver/gpio.h"
#include "driver/dedic_gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_sig_map.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXGPIO_PIN_MASK(pin) (1ULL<<pin)

typedef enum
{
    exgpio_iomux_out,
    exgpio_iomux_in,
} exgpio_iomux_mode_t;

void exgpio_init(uint64_t pin_mask, gpio_mode_t mode, gpio_pullup_t pullup, gpio_pulldown_t pulldown, gpio_int_type_t int_type);
void exgpio_toggle(gpio_num_t pin);
void exgpio_install_isr_service(void);
void exgpio_isr_handler_add(gpio_num_t pin, gpio_isr_t isr_handler, void *user_data);
void exgpio_set_iomux(gpio_num_t pin, exgpio_iomux_mode_t exgpio_iomux_mode, int iomux_param);
dedic_gpio_bundle_handle_t exgpio_dedic_gpio_bundle_init(int* gpio_array, size_t array_size, unsigned int out_en, unsigned int in_en);
void exgpio_dedic_bundle_deinit(dedic_gpio_bundle_handle_t bundle_handle);
void exgpio_dedic_gpio_set_level(dedic_gpio_bundle_handle_t bundle_handle, uint32_t offset, int value);
int exgpio_dedic_gpio_get_level(dedic_gpio_bundle_handle_t bundle_handle, uint32_t offset);

#ifdef __cplusplus
}
#endif