// fat16.c
// A FAT16 "minidriver". Basically just a practicerun...

#include <tros/fs/vfs.h>

void fat16_mount(fs_node_t* mountpoint);


static fs_operations_t fops =
{
    .fs_read = 0,
    .fs_write = 0,
    .fs_open = 0,
    .fs_close = 0,
    .fs_readdir = 0,
    .fs_finddir = 0,
    .fs_create = 0,
    .fs_delete = 0
};

int fat16_fs_initialize()
{
    filesystem_t fs =
    {
        .name = "fat16",
        .fops = &fops,
        .fs_mount = fat16_mount
    };
    return fs_register(&fs);
}

void fat16_mount(fs_node_t* mountpoint)
{
    
}
