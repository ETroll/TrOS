#include <stdlib.h>
#include "list.h"

static list_node_t* list_node_at(list_t* list, unsigned int index);

list_t* list_create()
{
    list_t* list = (list_t*)malloc(sizeof(list_t));
    if(list != NULL)
    {
        list->head = NULL;
        list->size = 0;
    }
    return list;
}

void list_free(list_t* list)
{
    if(list != NULL)
    {
        if(list->size > 0)
        {
            list_clear(list);
        }
        free(list);
    }
}

void list_add(list_t* list, void* data)
{
    list_node_t* node = (list_node_t*)malloc(sizeof(list_node_t));
    if(node)
    {
        node->data = data;
        node->next = NULL;
        node->prev = NULL;

        if(list->head == NULL)
        {
            list->head = node;
            list->tail = node;
        }
        else
        {
            node->prev = list->tail;
            list->tail->next = node;
            list->tail = node;
        }

        list->size++;
    }
}

void list_clear(list_t* list)
{
    foreach(i, list)
    {
        free(i);
    }
}

void list_remove_at(list_t* list, unsigned int index)
{
    if(list)
    {
        list_node_t* node = list_node_at(list, index);
        if(node != NULL)
        {
            if(node == list->head)
            {
                if(node->next == NULL)
                {
                    list->head = NULL;
                    list->tail = NULL;
                }
                else
                {
                    node->next->prev = NULL;
                    list->head = node->next;
                }
            }
            else if(node == list->tail)
            {
                node->prev->next = NULL;
                list->tail = node->prev;
            }
            else
            {
                node->next->prev = node->prev;
                node->prev->next = node->next;
            }
            free(node);
            list->size--;
        }
    }
}

void* list_get_at(list_t* list, unsigned int index)
{
    list_node_t* node = list_node_at(list, index);
    return node != NULL ? node->data : (void*)NULL;
}

list_node_t* list_node_at(list_t* list, unsigned int index)
{
    list_node_t* node = NULL;

    if(list)
    {
        node = list->head;
        for(uint32_t i = 0; (i < index) && node != NULL; i++, node = node->next);
    }
    return node;
}
