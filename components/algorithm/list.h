#ifndef __LIST_H
#define __LIST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct node_t
{
    void * data;
    struct node_t* next;
    struct node_t* last;
} node_t;

typedef struct list_t
{
    node_t* head;
    node_t* tail;
} list_t;

list_t* create_list(void);
node_t* create_node(unsigned int data_size);
void insert_node(list_t* list, node_t* node, int index);
void delete_node(list_t* list, node_t* node);
void delete_list(list_t* list);

#ifdef __cplusplus
}
#endif

#endif