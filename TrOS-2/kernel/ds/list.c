// llist.c
// Simple linked list implementation

#include <tros/ds/list.h>
#include <tros/kheap.h>


void list_add(list_t* list, void* data)
{
    list_node_t* new_node = kmalloc(sizeof(list_node_t));
    new_node->data = data;
    new_node->next = list->head;
    list->head = new_node;
    list->size++;
}

void list_add_back(list_t* list, void* data)
{
    list_node_t* new_node = kmalloc(sizeof(list_node_t));
    new_node->data = data;
    new_node->next = 0;

    if(list->head != 0)
    {
        list_node_t* tmp = list->head;
        while(tmp->next != 0)
        {
            tmp = tmp->next;
        }
        tmp->next = new_node;
    }
    else
    {
        list->head = new_node;
    }
    list->size++;
}

void list_remove_all(list_t* list)
{
    if(list->head != 0)
    {
        list_node_t* tmp = list->head;
        while(tmp->next != 0)
        {
            list->head = tmp->next;
            kfree(tmp);
            tmp = list->head;
        }
        kfree(tmp);
        list->head = 0;
        list->size = 0;
    }
}

list_node_t* list_remove(list_t* list, list_node_t* node)
{
    list_node_t* prev = 0;
    list_node_t* curr = list->head;

    while(curr != 0 && curr != node)
    {
        prev = curr;
        curr = curr->next;
    }

    if(curr != 0)
    {
        if(prev != 0)
        {
            prev->next = curr->next;
        }
        else
        {
            list->head = curr->next;
        }
        curr->next = 0;
        list->size--;
        return curr;
    }
    else
    {
        return 0;
    }
}

list_node_t* list_get_at_index(list_t* list, unsigned int index)
{
    if(index < list->size)
    {
        list_node_t* curr = list->head;
        for(int i = 0; i<index; i++)
        {
            curr = curr->next;
        }
        return curr;
    }
    else
    {
        return 0;
    }
}
