#include "list.h"
#include <stdlib.h>

list_t* create_list(void)
{
    list_t* list = malloc(sizeof(list_t));
    if(list)
    {
        list->head = NULL;
        list->tail = NULL;
        return list;
    }
    return NULL;
}

node_t* create_node(unsigned int data_size)
{
    node_t* node = malloc(sizeof(node_t));
    if(node)
    {
        node->data = malloc(data_size);
        if(node->data)
        {
        	node->next = NULL;
        	node->last = NULL;
        	return node;
		}
        free(node);
    }
    return NULL;
}

void insert_node(list_t* list, node_t* node, int index)
{
    if(list && node)
    {
        if(index < -1)
            index = -1;
        if(index == -1) // insert at tail
        {
            node_t* tail = list->tail;
            node->next = NULL;
            node->last = tail;
            if(tail)
                tail->next = node;
            else
                list->head = node;
            list->tail = node;
        }
        else if(index == 0) // insert at head
        {
            node_t* head = list->head;
            node->next = head;
            node->last = NULL;
            if(head)
                head->last = node;
            else
                list->tail = node;
            list->head = node;
        }
        else // insert at other place
        {
            node_t* temp = list->head;
            int i = 0;
            while(1)
            {
                if(i == index && temp)
                {
                    node_t* last = temp->last;
                    node->last = last;
                    node->next = temp;
                    temp->last = node;
                    last->next = node;
                    break;
                }
                if(!temp)
                {
                    insert_node(list, node, -1);
                    break;
                }
                temp = temp->next;
                ++i;
            }
        }
    }
}

void delete_node(list_t* list, node_t* node)
{
    if(list && node)
    {
        node_t* last = node->last;
        node_t* next = node->next;
        if(last)
        	last->next = next;
        else
        	list->head = next;
        if(next)
            next->last = last;
        else
        	list->tail = last;
        if(node->data)
            free(node->data);
        free(node);
        node = NULL;
    }
}

void delete_list(list_t* list)
{
    node_t* temp;
    if(list)
    {
        while(list->head)
        {
            temp = list->head;
            list->head = temp->next;
            free(temp);
        }
        free(list);
        list = NULL;
    }
}