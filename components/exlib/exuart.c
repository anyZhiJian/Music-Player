#include "exuart.h"

void exuart_init(uart_port_t uart_num, int baud_rate, int tx_io_num, int rx_io_num)
{
    ESP_ERROR_CHECK(uart_driver_install(uart_num, EXUART_BUFFER_SIZE, EXUART_BUFFER_SIZE, EXUART_QUEUE_SIZE, NULL, 0));
    uart_config_t uart_cfg = {
    .baud_rate = baud_rate,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_cfg));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, tx_io_num, rx_io_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

void exuart_deinit(uart_port_t uart_num)
{
    ESP_ERROR_CHECK(uart_driver_delete(uart_num));
}

int exuart_read(uart_port_t uart_num, void *buffer, size_t length, TickType_t ticks_to_wait)
{
    return uart_read_bytes(uart_num, buffer, length, ticks_to_wait);
}

void exuart_write(uart_port_t uart_num, const void *buffer, size_t length)
{
    size_t write = length;
    size_t writen = 0;
    while(write)
    {
        writen += uart_write_bytes(uart_num, (uint8_t*)buffer + writen, write);
        write = length - writen;
    }
}