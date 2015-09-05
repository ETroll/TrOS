// vfs.c
// Primitive VFS implementation
#include <tros/fs/vfs.h>
#include <tros/ds/list.h>
#include <tros/ds/tree.h>
#include <tros/kheap.h>
#include <tros/driver.h>
#include <tros/tros.h>
#include <string.h>

// ------
//  / (Root - System disk)
//  -- System/
//      -- Boot/
//      -- Lib/
//      -- Bin/
//  -- Applications/
//  -- Volumes/

tree_t* _vfs_tree = 0;

static fs_node_t* vfs_find_node(char* path);

void vfs_initialize()
{
    _vfs_tree = (tree_t*)kmalloc(sizeof(tree_t));
    fs_node_t* mnt = (fs_node_t*)kmalloc(sizeof(fs_node_t));

    strcpy(mnt->name, "/");
    mnt->inode = 0;
    mnt->size = 0;
    mnt->flags = VFS_FLAG_DIRECTORY;
    mnt->fsops = 0;
    mnt->device = 0;

    _vfs_tree->root = tree_node_create(mnt);
    _vfs_tree->size = 1;
}

unsigned int vfs_read(fs_node_t* inode, unsigned int offset, unsigned int size, unsigned char* buffer)
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

unsigned int vfs_write(fs_node_t* inode, unsigned int offset, unsigned int size, unsigned char* buffer)
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

void vfs_open(fs_node_t* inode)
{
    if(inode->fsops->fs_open != 0)
    {
        inode->fsops->fs_open(inode);
    }
}

void vfs_close(fs_node_t* inode)
{
    if(inode->fsops->fs_close != 0)// && inode != vfs_root)
    {
        inode->fsops->fs_close(inode);
    }
}

dirent_t* vfs_readdir(fs_node_t* inode, unsigned int index)
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

void vfs_create(char* name)
{

}

void vfs_delete(char* name)
{

}

int vfs_mount(char* device, char* fsname, char* path)
{
    device_driver_t* drv = driver_find_device(device);

    if(drv != 0 && drv->type == DRV_BLOCK)
    {
        filesystem_t* fs = fs_lookup(fsname);
        if(fs)
        {
            //TODO: Error handling for missing filesystem
            //TODO: Check if path is taken by another mountpoint
            //TODO: Canonicalize path!

            fs_node_t* node = vfs_find_node(path);
            if(node == 0)
            {
                //Example: /Volumes/CDROM
                // -> Then /Volumes/ must exist, if not then ERROR!
                //TODO: create node
            }

            if((node->flags & VFS_FLAG_DIRECTORY) && !(node->flags & VFS_FLAG_MOUNTPOINT))
            {
                node->flags |= VFS_FLAG_MOUNTPOINT;
                node->device = (driver_block_t*)drv->driver;
                node->fsops = fs->fsops;

                return fs->fs_mount(node);
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

static fs_node_t* vfs_find_node(char* path)
{
    //TODO: Actually return a node...
    return _vfs_tree->root->data;
}

fs_node_t* kopen(char* path)
{
    if(_vfs_tree && _vfs_tree->root)
    {
        unsigned int path_len = strlen(path);
        if(path_len == 1)
        {
            fs_node_t *vfs_root_clone = kmalloc(sizeof(fs_node_t));
            memcpy(vfs_root_clone, _vfs_tree->root->data, sizeof(fs_node_t));
            vfs_open(vfs_root_clone);

            return vfs_root_clone;
        }
        else
        {
            return 0; //only support root now :P
        }
    }
    else
    {
        return 0;
    }
}


/// FS functions
/// ---------------------
list_t* _fs_filesystems = (list_t*)EMPTY_LIST;

int fs_register(filesystem_t* fs)
{
    if(_fs_filesystems == (list_t*)EMPTY_LIST)
    {
        _fs_filesystems = (list_t*)kmalloc(sizeof(list_t));
        _fs_filesystems->head = 0;
        _fs_filesystems->size = 0;
    }

    filesystem_t* data = (filesystem_t*)kmalloc(sizeof(filesystem_t));
    data->name = (char*)kmalloc(strlen(fs->name)+1);

    strcpy(data->name, fs->name);
    data->fsops = fs->fsops;
    data->fs_mount = fs->fs_mount;
    list_add(_fs_filesystems, data);

    return _fs_filesystems->size;
}

filesystem_t* fs_lookup(char* name)
{
    list_node_t* node = _fs_filesystems->head;
    filesystem_t* fs = 0;

    while(node != 0)
    {
        filesystem_t* tmp = (filesystem_t*)node->data;
        if(strcmp(tmp->name, name) == 0)
        {
            fs = tmp;
            break;
        }
        node = node->next;
    }
    return fs;
}
