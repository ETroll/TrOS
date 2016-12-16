#ifndef INCLUDE_TROS_FS_H
#define INCLUDE_TROS_FS_H

#include <tros/fs/vfs.h>

typedef struct
{
    char* name;
    fs_operations_t* fsops;
    int (*mount)(fs_node_t* mountpoint);
} filesystem_t;

int fs_register(filesystem_t* fs);
filesystem_t* fs_lookup(char* name);

#endif
