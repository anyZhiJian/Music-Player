#pragma once

#include "exsocket.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TCP_BUFFER_SIZE 1024

typedef struct
{
    uint32_t port;
    int sock;
    uint32_t flag;
    uint32_t size;
    char data[TCP_BUFFER_SIZE];
} tcp_packet_t;

int tcp_server_task_begin(void);
int tcp_register_server(uint16_t port);
int tcp_server_send_data(uint32_t port, uint32_t sock, void* data, uint32_t size);
int tcp_server_recv_data(tcp_packet_t* packet, TickType_t wait_ticks);
int tcp_client_task_begin(void);
int tcp_register_client(const char *ip, uint16_t port);
int tcp_client_send_data(uint32_t port, void* data, uint32_t size);
int tcp_client_recv_data(tcp_packet_t* packet, TickType_t wait_ticks);
int tcp_server_send_all(uint32_t port, void* data, uint32_t size);
int tcp_get_connection_num(uint32_t port);

#ifdef __cplusplus
}
#endif