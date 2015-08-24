// vfs.c
// Primitive VFS implementation
#include <tros/fs/vfs.h>

// ------
//  / (Root - System disk)
//  -- System/
//      -- Boot/
//      -- Lib/
//      -- Bin/
//  -- Applications/
//  -- Volumes/

void vfs_initialize()
{

}

unsigned int vfs_read(vfs_node_t* inode, unsigned int offset, unsigned int size, unsigned char* buffer)
{
    return 0;
}

unsigned int vfs_write(vfs_node_t* inode, unsigned int offset, unsigned int size, unsigned char* buffer)
{
    return 0;
}

void vfs_open(vfs_node_t* inode)
{

}

void vfs_close(vfs_node_t* inode)
{

}

struct vfs_dirent* vfs_readdir(vfs_node_t* inode, unsigned int index)
{
    return 0;
}

vfs_node_t* vfs_finddir(vfs_node_t* inode, char* name)
{
    return 0;
}

void vfs_create_file(char* name)
{

}

void vfs_delete_file(char* name)
{

}

void vfs_mkdir(char* name)
{

}

void vfs_rmdir(char* name)
{

}
