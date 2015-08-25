// vfs.h
// Virtual File System

#ifndef INCLUDE_TROS_VFS_H
#define INCLUDE_TROS_VFS_H

#include <tros/driver.h>

#define FILE_NAME_MAX 256

struct vfs_node;
struct vfs_dirent;

typedef struct
{
    unsigned int (*fs_read)(struct vfs_node* inode, unsigned int offset, unsigned int size, unsigned char* buffer);
    unsigned int (*fs_write)(struct vfs_node* inode, unsigned int offset, unsigned int size, unsigned char* buffer);
    void (*fs_open)(struct vfs_node* inode);
    void (*fs_close)(struct vfs_node* inode);
    struct vfs_dirent* (*fs_readdir)(struct vfs_node* inode, unsigned int index); //Needed?
    struct vfs_node* (*fs_finddir)(struct vfs_node* inode, char* name);
    void (*fs_create)(struct vfs_node* inode, char* name);
    void (*fs_delete)(struct vfs_node* inode);
} fs_operations_t;

typedef struct vfs_node
{
    char name[FILE_NAME_MAX];
    unsigned int inode;
    unsigned int size;
    unsigned int flags;
    fs_operations_t* fsops;
    driver_block_t* device;

} vfs_node_t;

struct vfs_dirent
{
    char name[FILE_NAME_MAX];
    unsigned int inodenum;
};

typedef struct
{
    char* name;
    fs_operations_t* fops;
    void (*fs_super)();         //TODO: Need something here. How should it init?

} filesystem_t;

void vfs_initialize();

unsigned int vfs_read(vfs_node_t* inode, unsigned int offset, unsigned int size, unsigned char* buffer);
unsigned int vfs_write(vfs_node_t* inode, unsigned int offset, unsigned int size, unsigned char* buffer);
void vfs_open(vfs_node_t* inode);
void vfs_close(vfs_node_t* inode);
struct vfs_dirent* vfs_readdir(vfs_node_t* inode, unsigned int index);
vfs_node_t* vfs_finddir(vfs_node_t*, char* name);
void vfs_create(char* name);
void vfs_delete(char* name);


//Maybe separate out in own FS.c/h?
int fs_register(filesystem_t* fs);
int fs_mount(char* device, char* fsname, char* path);

#endif
