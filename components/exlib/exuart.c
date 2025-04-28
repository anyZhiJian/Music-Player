#include "exuart.h"

QueueHandle_t exuart_init(uart_port_t uart_num, int baud_rate, int tx_io_num, int rx_io_num)
{
    uart_config_t uart_config = {
    .baud_rate = baud_rate,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, tx_io_num, rx_io_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    QueueHandle_t uart_queue = NULL;
    ESP_ERROR_CHECK(uart_driver_install(uart_num, EXUART_BUFFER_SIZE, EXUART_BUFFER_SIZE, QUEUE_SIZE, &uart_queue, 0));
    return uart_queue;
}

int exuart_queue_handler(uart_port_t uart_num, QueueHandle_t queue_handle, void* buffer, int echo, uint32_t wait_ticks)
{
    int ret = 0;
    uart_event_t event = {0};
    if(pdTRUE == xQueueReceive(queue_handle, &event, wait_ticks))
    {
        switch(event.type)
        {
            case UART_DATA:
            {
                ret = uart_read_bytes(uart_num, buffer, EXUART_BUFFER_SIZE, 0);
                if(echo && ret > 0)
                    uart_write_bytes(uart_num, buffer, ret);
            }
            break;
            case UART_BUFFER_FULL:
                ESP_ERROR_CHECK(uart_flush_input(uart_num));
                xQueueReset(queue_handle);
            break;
            case UART_FIFO_OVF:
                ESP_ERROR_CHECK(uart_flush_input(uart_num));
                xQueueReset(queue_handle);
            default:break;
        }
    }
    return ret;
}