// llist.c
// Simple linked list implementation

#include <tros/klib/list.h>
#include <tros/memory.h>


static list_node_t* list_node_at(list_t* list, unsigned int index);

list_t* list_create()
{
    list_t* list = (list_t*)kmalloc(sizeof(list_t));
    if(list != 0)
    {
        list->head = 0;
        list->size = 0;
    }
    return list;
}

void list_dispose(list_t* list)
{
    if(list != 0)
    {
        if(list->size > 0)
        {
            list_clear(list);
        }
        kfree(list);
    }
}

void list_add(list_t* list, void* data)
{
    list_node_t* node = (list_node_t*)kmalloc(sizeof(list_node_t));
    if(node)
    {
        node->data = data;
        node->next = 0;
        node->prev = 0;

        if(list->head == 0)
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
        kfree(i);
    }
}

void list_remove_at(list_t* list, unsigned int index)
{
    if(list)
    {
        list_node_t* node = list_node_at(list, index);
        if(node != 0)
        {
            if(node == list->head)
            {
                if(node->next == 0)
                {
                    list->head = 0;
                    list->tail = 0;
                }
                else
                {
                    node->next->prev = 0;
                    list->head = node->next;
                }
            }
            else if(node == list->tail)
            {
                node->prev->next = 0;
                list->tail = node->prev;
            }
            else
            {
                node->next->prev = node->prev;
                node->prev->next = node->next;
            }
            kfree(node);
            list->size--;
        }
    }
}

void* list_get_at(list_t* list, unsigned int index)
{
    list_node_t* node = list_node_at(list, index);
    return node != 0 ? node->data : (void*)0;
}

list_node_t* list_node_at(list_t* list, unsigned int index)
{
    list_node_t* node = 0;

    if(list)
    {
        node = list->head;
        for(uint32_t i = 0; (i < index) && node != 0; i++, node = node->next);
    }
    return node;
}
