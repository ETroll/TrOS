// vfs.c
// Primitive VFS implementation
#include <tros/fs/vfs.h>
#include <tros/fs.h>
#include <tros/ds/tree.h>
#include <tros/kheap.h>
#include <tros/tros.h>
#include <string.h>

/*
    /
        fd0/
            initrd
            kernel.elf
            krnldr.bin
            test
            folder/
                test1
                test2
                ...
        hd0/
        cd0/
        dvd0/
*/
tree_node_t* _vfs_root = 0;

static fs_node_t* vfs_find_node(char* path);
static fs_node_t* vfs_copy_node(fs_node_t* node);
static list_t* vfs_tokenize_path(char* path);
static fs_node_t* vfs_node_from_dir(fs_node_t* parent, dirent_t* dir);

void vfs_initialize()
{
    fs_node_t* mnt = (fs_node_t*)kmalloc(sizeof(fs_node_t));

    strcpy(mnt->name, "/");
    mnt->inode = 0;
    mnt->size = 0;
    mnt->flags = VFS_FLAG_ROOTDIR;
    mnt->fsops = 0;
    mnt->device = 0;

    _vfs_root = tree_node_create(mnt);
}

unsigned int vfs_read(fs_node_t* inode, unsigned int offset, unsigned int size, unsigned char* buffer)
{
    //NOTE: Add guards against bad params?
    //      - offset + size > filesize
    int bytesread = 0;
    if(inode && (inode->flags && VFS_FLAG_FILE) && inode->fsops->fs_read != 0)
    {
        bytesread = inode->fsops->fs_read(inode, offset, size, buffer);
    }
    return bytesread;
}

unsigned int vfs_write(fs_node_t* inode, unsigned int offset, unsigned int size, unsigned char* buffer)
{
    int byteswritten = 0;
    if(inode && (inode->flags && VFS_FLAG_FILE) && inode->fsops->fs_write != 0)
    {
        byteswritten = inode->fsops->fs_write(inode, offset, size, buffer);
    }
    return byteswritten;
}

void vfs_open(fs_node_t* inode)
{
    if(inode && inode->fsops && inode->fsops->fs_open != 0)
    {
        inode->fsops->fs_open(inode);
    }
}

void vfs_close(fs_node_t* inode)
{
    if(inode && inode->fsops && inode->fsops->fs_close != 0)// && inode != vfs_root)
    {
        inode->fsops->fs_close(inode);
    }
}

dirent_t* vfs_readdir(fs_node_t* inode, unsigned int index)
{
    //printk("IN:%x I:%d ", inode, index);
    dirent_t* entry = 0;
    if(inode)
    {
        if((inode->flags & VFS_FLAG_ROOTDIR))
        {
            //printk("R ");
            if(_vfs_root != 0)
            {
                tree_node_t* child = tree_get_child_index(_vfs_root, index);
                //printk("C:%x ", child);
                if(child)
                {
                    fs_node_t* mnt = (fs_node_t*)child->data;
                    //printk("D:%x N: %s ", mnt, mnt->name);

                    entry = (dirent_t*)kmalloc(sizeof(dirent_t));
                    strcpy(entry->name, mnt->name);
                    entry->inodenum = mnt->inode;
                    entry->flags = mnt->flags;
                }
            }
        }
        else if((inode->flags & VFS_FLAG_DIRECTORY) && inode->fsops->fs_readdir)
        {
            //printk("DR ");
            entry = inode->fsops->fs_readdir(inode, index);
        }
    }
    //printk("\n");
    return entry;
}

void vfs_create(char* name)
{

}

void vfs_delete(char* name)
{

}

int vfs_mount(char* device, char* fsname)
{
    int retcode = 0;
    device_driver_t* drv = driver_find_device(device);

    if(drv != 0 && drv->type == DRV_BLOCK)
    {
        filesystem_t* fs = fs_lookup(fsname);
        if(fs)
        {
            fs_node_t* mnt = (fs_node_t*)kmalloc(sizeof(fs_node_t));

            strcpy(mnt->name, drv->name);
            mnt->flags = VFS_FLAG_DIRECTORY | VFS_FLAG_MOUNTPOINT;
            mnt->fsops = fs->fsops;
            mnt->device = (driver_block_t*)drv->driver;

            if(fs->mount(mnt))
            {
                tree_node_insert(_vfs_root, tree_node_create(mnt));
                retcode = 1;
            }
        }
    }
    return retcode;
}

static fs_node_t* vfs_find_node(char* path)
{
    fs_node_t* node = 0;
    unsigned int path_len = strlen(path);

    if(path_len > 0 && path[0] == PATH_DELIMITER)
    {
        if(path_len == 1)
        {
            node = vfs_copy_node((fs_node_t*)_vfs_root->data);
        }
        else
        {
            list_t* tokenlist = vfs_tokenize_path(path);
            if(tokenlist->size > 0)
            {
                //printk("Got %d tokens\n", tokenlist->size);

                fs_node_t* deviceroot = 0;
                char* devicename = tokenlist->head->data;

                for(int i = 0; i<_vfs_root->children->size; i++)
                {
                    //TODO: replace usage of tree_get_child_index with more effieient way
                    fs_node_t* tmp = tree_get_child_index(_vfs_root, i)->data;
                    if(strcmp(tmp->name, devicename) == 0)
                    {
                        deviceroot = tmp;
                        break;
                    }
                }

                if(deviceroot)
                {
                    //printk("Found root: %s\n", deviceroot->name);
                    if(tokenlist->size == 1)
                    {
                        node = vfs_copy_node(deviceroot);
                    }
                    else
                    {
                        fs_node_t* current_folder = vfs_copy_node(deviceroot);
                        unsigned int failsafe = 0;
                        list_node_t* target = tokenlist->head->next;

                        vfs_open(deviceroot);
                        while(failsafe < 300)
                        {
                            fs_node_t* foundnode = 0;
                            unsigned int index = 0;

                            dirent_t* dirent = vfs_readdir(current_folder, index);
                            while (dirent != 0)
                            {
                                //printk("Comparing entry: %s with target: %s\n", dirent->name, target->data);
                                if(strcmp(dirent->name, target->data) == 0)
                                {
                                    foundnode = vfs_node_from_dir(deviceroot, dirent);
                                    kfree(dirent);
                                    break;
                                }
                                else
                                {
                                    kfree(dirent);
                                    dirent = vfs_readdir(current_folder, ++index);
                                }
                            }

                            if(foundnode)
                            {
                                if(target->next)
                                {
                                    target = target->next;
                                    kfree(current_folder);
                                    current_folder = foundnode;
                                    failsafe++;
                                }
                                else
                                {
                                    //Last node, we have a match!!
                                    node = foundnode;
                                    break;
                                }
                            }
                            else
                            {
                                //Could not find file
                                printk("Could not find %s. Giving up!\n", target->data);
                                break;
                            }
                        }
                        vfs_close(deviceroot);
                    }
                }
                else { printk("Did not find any device root\n");}
            }
            else { printk("Got NO tokens\n");}

            list_remove_all(tokenlist);
            kfree(tokenlist);
        }
    }
    //printk("\n");
    return node;
}

//NOTE: Need _FULL_ path
fs_node_t* kopen(char* path)
{
    fs_node_t* node = 0;
    if(_vfs_root)
    {
        node = vfs_find_node(path);
        if(node)
        {
            vfs_open(node);
        }
    }
    return node;
}

fs_node_t* vfs_copy_node(fs_node_t* node)
{
    fs_node_t* newnode = (fs_node_t*)kmalloc(sizeof(fs_node_t));
    memcpy(newnode, node, sizeof(fs_node_t));
    return newnode;
}

list_t* vfs_tokenize_path(char* path)
{
    unsigned int lastmatch = 1;
    unsigned int nextmatch = 1;
    unsigned int depth = 0;
    unsigned int path_len = strlen(path);

    list_t* tokens = (list_t*)kmalloc(sizeof(list_t));
    tokens->head = 0;
    tokens->size = 0;

    while (depth++ < 255)
    {
        for(int i = lastmatch; i<path_len; i++)
        {
            if(path[i] == PATH_DELIMITER)
            {
                nextmatch = i+1;
                break;
            }
        }
        if(nextmatch == lastmatch)
        {
            nextmatch = path_len;
        }

        unsigned int tokensize = nextmatch-lastmatch;
        if(tokensize > 0 && !(tokensize == 1 && path[lastmatch] == PATH_DELIMITER))
        {
            char* token = (char*)kmalloc(tokensize + 1);
            strncpy(token, (path+lastmatch), tokensize);

            if(token[tokensize-1] == PATH_DELIMITER)
            {
                //TODO: Flag as folder?
                token[tokensize-1] = '\0';
            }
            else
            {
                token[tokensize] = '\0';
            }
            list_add(tokens, token);
        }

        if(nextmatch >= path_len)
        {
            break;
        }
        else
        {
            lastmatch = nextmatch;
        }
    }
    return tokens;
}

fs_node_t* vfs_node_from_dir(fs_node_t* parent, dirent_t* dir)
{
    fs_node_t* node = 0;

    if(dir)
    {
        node = (fs_node_t*)kmalloc(sizeof(fs_node_t));
        strcpy(node->name, dir->name);
        node->inode = dir->inodenum;
        node->flags = dir->flags;
        node->size = 0;
        node->fsops = parent->fsops;
        node->device = parent->device;
    }


    return node;
}
