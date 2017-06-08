// A better list. Maybe re-use in a library

// Uses pointers as payload / data. So this needs to be freed outside of
// the list.

#ifndef INCLUDE_LIST_H
#define INCLUDE_LIST_H

typedef struct list_node
{
    void* data;
    struct list_node* next;
    struct list_node* prev;
} list_node_t;

typedef struct
{
    list_node_t* head;
    list_node_t* tail;
    unsigned int size;
} list_t;

#define foreach(i, list) for (list_node_t* i = list->head; i != 0; i = i->next)

list_t* list_create();
void list_free(list_t* list);

void list_add(list_t* list, void* data);
void list_clear(list_t* list);
void list_remove_at(list_t* list, unsigned int index);
void* list_get_at(list_t* list, unsigned int index);

#endif
