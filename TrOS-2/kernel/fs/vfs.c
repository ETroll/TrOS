// vfs.c
// Primitive VFS implementation
#include <tros/fs/vfs.h>
#include <tros/ds/list.h>
#include <tros/kheap.h>
#include <string.h>

// ------
//  / (Root - System disk)
//  -- System/
//      -- Boot/
//      -- Lib/
//      -- Bin/
//  -- Applications/
//  -- Volumes/

vfs_node_t *vfs_root = 0;

enum VFS_FLAGS
{
    VFS_FLAG_FILE       = 0x01,
    VFS_FLAG_DIRECTORY  = 0x02
};


void vfs_initialize()
{
    //TODO: Set up root node
}

unsigned int vfs_read(vfs_node_t* inode, unsigned int offset, unsigned int size, unsigned char* buffer)
{
    if(inode->fsops->fs_read != 0)
    {
        return inode->fsops->fs_read(inode, offset, size, buffer);
    }
    else
    {
        return 0;
    }
}

unsigned int vfs_write(vfs_node_t* inode, unsigned int offset, unsigned int size, unsigned char* buffer)
{
    if(inode->fsops->fs_write != 0)
    {
        return inode->fsops->fs_write(inode, offset, size, buffer);
    }
    else
    {
        return 0;
    }
}

void vfs_open(vfs_node_t* inode)
{
    if(inode->fsops->fs_open != 0)
    {
        inode->fsops->fs_open(inode);
    }
}

void vfs_close(vfs_node_t* inode)
{
    if(inode->fsops->fs_close != 0 && inode != vfs_root)
    {
        inode->fsops->fs_close(inode);
    }
}

struct vfs_dirent* vfs_readdir(vfs_node_t* inode, unsigned int index)
{
    if((inode->flags & VFS_FLAG_DIRECTORY) && inode->fsops->fs_readdir)
    {
        return inode->fsops->fs_readdir(inode, index);
    }
    else
    {
        return 0;
    }
}

vfs_node_t* vfs_finddir(vfs_node_t* inode, char* name)
{
    if((inode->flags & VFS_FLAG_DIRECTORY) && inode->fsops->fs_finddir)
    {
        return inode->fsops->fs_finddir(inode, name);
    }
    else
    {
        return 0;
    }
}

void vfs_create(char* name)
{

}

void vfs_delete(char* name)
{

}


/// FS functions
/// ---------------------
list_t* _fs_filesystems = 0;

int fs_register(filesystem_t* fs)
{
    if(_fs_filesystems == 0)
    {
        _fs_filesystems = (list_t*)kmalloc(sizeof(list_t));
        _fs_filesystems->head = 0;
        _fs_filesystems->size = 0;
    }

    filesystem_t* data = (filesystem_t*)kmalloc(sizeof(filesystem_t));
    strcpy(data->name, fs->name);
    data->fops = fs->fops;
    data->fs_super = fs->fs_super;

    list_add(_fs_filesystems, data);

    return _fs_filesystems->size-1;
}

int fs_mount(char* device, char* fsname, char* path)
{
    return 0;
}
