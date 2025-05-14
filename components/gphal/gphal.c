#include "gphal.h"

volatile uint32_t gphal_ticks;

inline void gphal_tick_handler(uint16_t ticks)
{
    gphal_ticks += ticks;
}

inline uint32_t gphal_get_ticks(void)
{
    return gphal_ticks;
}