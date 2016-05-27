// tree.h
// Basic tree implementation

#ifndef INCLUDE_DS_TREE_H
#define INCLUDE_DS_TREE_H

#include <tros/ds/list.h>

typedef struct tree_node
{
    void* data;
    list_t* children;
    struct tree_node* parent;
} tree_node_t;

typedef struct
{
    tree_node_t* root;
    unsigned int size;
} tree_t;

void tree_node_free(tree_node_t* node);
tree_node_t* tree_node_create(void* data);
void tree_node_insert(tree_node_t* parent, tree_node_t* node);
tree_node_t* tree_get_child_index(tree_node_t* parent, unsigned int index);


#endif
