#include "tcp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "algorithm/list.h"

#define TCP_SOCK(node) (*((int*)node->data))
#define TCP_FLAG_SEND_ALL 0x01

typedef struct
{
    int sock;
    list_t *list_client;
    uint16_t port;
} tcp_server_t;

typedef struct
{
    int sock;
    uint16_t port;
} tcp_client_t;

static list_t* list_server;
static list_t* list_client;
static TaskHandle_t task_server_accept;
static TaskHandle_t task_server_send;
static TaskHandle_t task_server_recv;
static TaskHandle_t task_client_send;
static TaskHandle_t task_client_recv;
static QueueHandle_t queue_server_send;
static QueueHandle_t queue_server_recv;
static QueueHandle_t queue_client_send;
static QueueHandle_t queue_client_recv;
static tcp_packet_t buffer_server_send;
static tcp_packet_t buffer_server_recv;
static tcp_packet_t buffer_client_send;
static tcp_packet_t buffer_client_recv;

int tcp_register_server(uint16_t port)
{
    int ret = 0;
    if(!list_server)
    {
        ret = -1;
        goto err;
    }
    node_t* node = create_node(sizeof(tcp_server_t));
    if(!node)
    {
        ret = -2;
        goto err;
    }
    tcp_server_t* server = node->data;
    server->list_client = create_list();
    if(!server->list_client)
    {
        ret = -3;
        goto err;
    }
    server->sock = exsocket_tcp_server_init(port);
    if(server->sock <= 0)
    {
        ret = -4;
        goto err;
    }
    server->port = port;
    if(exsocket_set_nonblock(server->sock) != 0)
    {
        ret = -5;
        goto err;
    }
    insert_node(list_server, node, -1);
    return 0;
err:
    switch(ret)
    {
        case -5:close(server->sock);[[fallthrough]];
        case -4:delete_list(server->list_client);[[fallthrough]];
        case -3:free(node->data);free(node);[[fallthrough]];
        case -2:[[fallthrough]];
        case -1:[[fallthrough]];
        default:break;
    }
    return ret;
}

int tcp_register_client(const char *ip, uint16_t port)
{
    int ret = 0;
    if(!list_client)
    {
        ret = -1;
        goto err;
    }
    node_t* node = create_node(sizeof(tcp_client_t));
    if(!node)
    {
        ret = -2;
        goto err;
    }
    tcp_client_t* client = node->data;
    client->sock = exsocket_tcp_client_init(ip, port);
    if(client->sock <= 0)
    {
        ret = -3;
        goto err;
    }
    client->port = port;
    int temp = 0;
    temp |= exsocket_set_nonblock(client->sock);
    temp |= exsocket_tcp_set_nodelay(client->sock);
    temp |= exsocket_tcp_set_keepalive(client->sock);
    if(temp != 0)
    {
        ret = -4;
        goto err;
    }
    insert_node(list_client, node, -1);
    return 0;
err:
    switch(ret)
    {
        case -4:close(client->sock);[[fallthrough]];
        case -3:free(node->data);free(node);[[fallthrough]];
        case -2:[[fallthrough]];
        case -1:[[fallthrough]];
        default:break;
    }
    return ret;
}

static void tcp_server_accept(void* param)
{
    while(1)
    {
        node_t* temp = list_server->head;
        while(temp)
        {
            tcp_server_t* server = temp->data;
            temp = temp->next;
            int client_fd = exsocket_accept(server->sock);
            if(client_fd > 0)
            {
                int ret = 0;
                ret |= exsocket_set_nonblock(client_fd);
                ret |= exsocket_tcp_set_nodelay(client_fd);
                ret |= exsocket_tcp_set_keepalive(client_fd);
                if(ret == 0)
                {
                    node_t* client = create_node(sizeof(int));
                    if(client)
                    {
                        TCP_SOCK(client) = client_fd;
                        insert_node(server->list_client, client, -1);
                    }
                }
                else
                {
                    close(client_fd);
                }  
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void tcp_server_send(void* param)
{
    while(1)
    {
        if(xQueueReceive(queue_server_send, &buffer_server_send, pdMS_TO_TICKS(portMAX_DELAY)))
        {
            node_t* temp1 = list_server->head;
            while(temp1)
            {
                node_t* node_server = temp1;
                temp1 = temp1->next;
                tcp_server_t* server = node_server->data;

                if(buffer_server_send.port == server->port)
                {
                    node_t* temp2 = server->list_client->head;
                    while(temp2)
                    {
                        node_t* node_client = temp2;
                        temp2 = temp2->next;
                        int client_fd = TCP_SOCK(node_client);
                        
                        if(buffer_server_send.sock == client_fd || buffer_server_send.flag & TCP_FLAG_SEND_ALL)
                        {
                            exsocket_send(client_fd, buffer_server_send.data, buffer_server_send.size);
                            goto send_complete;
                        }
                    }
                }
            }
        }
send_complete:
    }
}

static void tcp_server_recv(void* param)
{
    while(1)
    {
        node_t* temp1 = list_server->head;
        while(temp1)
        {
            node_t* node_server = temp1;
            temp1 = temp1->next;
            tcp_server_t* server = node_server->data;

            node_t* temp2 = server->list_client->head;
            while(temp2)
            {
                node_t* node_client = temp2;
                temp2 = temp2->next;
                int client_fd = TCP_SOCK(node_client);

                int ret = exsocket_recv(client_fd, buffer_server_recv.data, TCP_BUFFER_SIZE);
                if(ret > 0)
                {
                    buffer_server_recv.size = ret;
                    buffer_server_recv.port = server->port;
                    buffer_server_recv.sock = client_fd;
                    xQueueSend(queue_server_recv, &buffer_server_recv, 0);
                }
                else if(ret < 0)
                {
                    close(client_fd);
                    delete_node(server->list_client, node_client);
                    printf("port[%d] clinet[%d] disconnected\r\n", server->port, client_fd);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void tcp_client_send(void* param)
{
    while(1)
    {
        if(xQueueReceive(queue_client_send, &buffer_client_send, pdMS_TO_TICKS(portMAX_DELAY)))
        {
            node_t* temp1 = list_client->head;
            while(temp1)
            {
                node_t* node_client = temp1;
                temp1 = temp1->next;
                tcp_client_t* client = node_client->data;
                int client_fd = client->sock;

                if(buffer_client_send.port == client->port)
                {
                    exsocket_send(client_fd, buffer_client_send.data, buffer_client_send.size);
                    goto send_complete;
                }
            }
        }
send_complete:
    }
}

static void tcp_client_recv(void* param)
{
    while(1)
    {
        node_t* temp1 = list_client->head;
        while(temp1)
        {
            node_t* node_client = temp1;
            temp1 = temp1->next;
            tcp_client_t* client = node_client->data;
            int client_fd = client->sock;

            int ret = exsocket_recv(client_fd, buffer_client_recv.data, TCP_BUFFER_SIZE);
            if(ret > 0)
            {
                buffer_client_recv.size = ret;
                buffer_client_recv.port = client->port;
                xQueueSend(queue_client_recv, &buffer_client_recv, 0);
            }
            else if(ret < 0)
            {
                close(client_fd);
                delete_node(list_client, node_client);
                printf("port[%d] server disconnected\r\n", client->port);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

int tcp_server_task_begin(void)
{
    if(!list_server && !task_server_accept && !task_server_send && !task_server_recv && !queue_server_send && !queue_server_recv)
    {
        list_server = create_list();
        if(list_server)
        {
            queue_server_send = xQueueCreate(5, sizeof(tcp_packet_t));
            queue_server_recv = xQueueCreate(5, sizeof(tcp_packet_t));
            xTaskCreate(tcp_server_accept, "", 4096, NULL, 10, &task_server_accept);
            xTaskCreate(tcp_server_send, "", 4096, NULL, 9, &task_server_send);
            xTaskCreate(tcp_server_recv, "", 4096, NULL, 9, &task_server_recv);
            return 0;
        }
    }
    return -1;
}

int tcp_client_task_begin(void)
{
    if(!list_client && !task_client_send && !task_client_recv && !queue_client_send && !queue_client_recv)
    {
        list_client = create_list();
        if(list_client)
        {
            queue_client_send = xQueueCreate(5, sizeof(tcp_packet_t));
            queue_client_recv = xQueueCreate(5, sizeof(tcp_packet_t));
            xTaskCreate(tcp_client_send, "", 4096, NULL, 9, &task_client_send);
            xTaskCreate(tcp_client_recv, "", 4096, NULL, 9, &task_client_recv);
        }
        return 0;
    }
    return -1;
}

int tcp_server_send_data(uint32_t port, uint32_t sock, void* data, uint32_t size)
{
    if(queue_server_send && data)
    {
        tcp_packet_t packet;
        packet.port = port;
        packet.sock = sock;
        if(size > TCP_BUFFER_SIZE)
        {
            size = TCP_BUFFER_SIZE;
        }
        packet.size = size;
        memcpy(packet.data, data, size);
        return xQueueSend(queue_server_send, &packet, 0);
    }
    return -1;
}

int tcp_server_recv_data(tcp_packet_t* packet, TickType_t wait_ticks)
{
    if(queue_server_recv && packet)
    {
        return xQueueReceive(queue_server_recv, packet, wait_ticks);
    }
    return -1;
}

int tcp_client_send_data(uint32_t port, void* data, uint32_t size)
{
    if(queue_client_send && data)
    {
        tcp_packet_t packet;
        packet.port = port;
        if(size > TCP_BUFFER_SIZE)
        {
            size = TCP_BUFFER_SIZE;
        }
        packet.size = size;
        memcpy(packet.data, data, size);
        return xQueueSend(queue_client_send, &packet, 0);
    }
    return -1;
}

int tcp_client_recv_data(tcp_packet_t* packet, TickType_t wait_ticks)
{
    if(queue_client_recv && packet)
    {
        return xQueueReceive(queue_client_recv, packet, wait_ticks);
    }
    return -1;
}

int tcp_server_send_all(uint32_t port, void* data, uint32_t size)
{
    if(queue_server_send && data)
    {
        tcp_packet_t packet;
        packet.port = port;
        packet.flag = TCP_FLAG_SEND_ALL;
        if(size > TCP_BUFFER_SIZE)
        {
            size = TCP_BUFFER_SIZE;
        }
        packet.size = size;
        memcpy(packet.data, data, size);
        return xQueueSend(queue_server_send, &packet, 0);
    }
    return -1;
}

int tcp_get_connection_num(uint32_t port)
{
    if(list_server)
    {
        node_t* temp1 = list_server->head;
        while(temp1)
        {
            tcp_server_t* server = temp1->data;
            temp1 = temp1->next;
            if(server->port == port)
            {
                node_t* temp2 = server->list_client->head;
                int cnt = 0;
                while(temp2)
                {
                    ++cnt;
                    temp2 = temp2->next;
                }
                return cnt;
            }
        }
    }
    return 0;
}