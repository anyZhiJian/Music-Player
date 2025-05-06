#pragma once

#include "driver/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXUART_QUEUE_SIZE   10
#define EXUART_BUFFER_SIZE  (1024 * 16)

void exuart_init(uart_port_t uart_num, int baud_rate, int tx_io_num, int rx_io_num);
void exuart_deinit(uart_port_t uart_num);
int exuart_read(uart_port_t uart_num, void *buffer, size_t length, TickType_t ticks_to_wait);
void exuart_write(uart_port_t uart_num, const void *buffer, size_t length);

#ifdef __cplusplus
}
#endif