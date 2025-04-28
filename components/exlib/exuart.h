#pragma once

#include "driver/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QUEUE_SIZE          10
#define EXUART_BUFFER_SIZE  (1024 * 2)

QueueHandle_t exuart_init(uart_port_t uart_num, int baud_rate, int tx_io_num, int rx_io_num);
int exuart_queue_handler(uart_port_t uart_num, QueueHandle_t queue_handle, void* buffer, int echo, uint32_t wait_ticks);

#ifdef __cplusplus
}
#endif