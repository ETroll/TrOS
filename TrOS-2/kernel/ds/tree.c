// tree.c
// Simple generic tree implementation

#include <tros/ds/tree.h>
#include <tros/kheap.h>

tree_node_t* tree_node_create(void* data)
{
    tree_node_t* tmp = (tree_node_t*)kmalloc(sizeof(tree_node_t));
    tmp->data = data;
    tmp->parent = 0;
    tmp->children = (list_t*)kmalloc(sizeof(list_t));
    tmp->children->head = 0;
    tmp->children->size = 0;

    return tmp;
}

void tree_node_free(tree_node_t* node)
{
    if(node)
    {
        for (list_node_t* i = node->children->head; i != 0; i = i->next)
        {
            tree_node_free((tree_node_t*)i->data);
        }
        if(node->children)
        {
            kfree(node->children);
        }
        kfree(node);
    }
}

void tree_node_insert(tree_t* tree, tree_node_t* parent, tree_node_t* node)
{
    list_add(parent->children, node);
    node->parent = parent;
    tree->size++;
}
