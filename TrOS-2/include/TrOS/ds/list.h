// llist.h
// Simple Linked list

#ifndef INCLUDE_DS_LIST_H
#define INCLUDE_DS_LIST_H

//#define foreach(i, list) for (list_node_t* i = list->head; i != 0; i = i->next)

typedef struct list_node
{
    void* data;
    struct list_node* next;
} list_node_t;

typedef struct
{
    list_node_t* head;
    unsigned int size;
} list_t;

void list_add(list_t* list, void* data);
void list_add_back(list_t* list, void* data);
void list_remove_all(list_t* list);
void list_node_remove(list_t* list, list_node_t* node);
list_node_t* list_get_at_index(list_t* list, unsigned int index);



#endif
