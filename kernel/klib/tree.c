// tree.c
// Simple generic tree implementation

#include <tros/klib/tree.h>
#include <tros/memory.h>

tree_node_t* tree_node_create(void* data)
{
    tree_node_t* tmp = (tree_node_t*)kmalloc(sizeof(tree_node_t));
    tmp->data = data;
    tmp->parent = 0;
    tmp->children = list_create();
    tmp->children->head = 0;
    tmp->children->size = 0;

    return tmp;
}

void tree_node_free(tree_node_t* node)
{
    if(node)
    {
        if(node->children)
        {
            foreach(i, node->children)
            {
                tree_node_free((tree_node_t*)i->data);
            }
            list_dispose(node->children);
        }
        kfree(node);
    }
}

void tree_node_insert(tree_node_t* parent, tree_node_t* node)
{
    list_add(parent->children, node);
    node->parent = parent;
}

tree_node_t* tree_get_child_index(tree_node_t* parent, unsigned int index)
{
    return (tree_node_t*)list_get_at(parent->children, index);
}
