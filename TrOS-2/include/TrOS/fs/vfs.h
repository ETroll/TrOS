// vfs.h
// Virtual File System

#ifndef INCLUDE_TROS_VFS_H
#define INCLUDE_TROS_VFS_H

#include <tros/driver.h>

#define FILE_NAME_MAX 256

struct fs_node;
struct vfs_dirent;

typedef struct
{
    unsigned int (*fs_read)(struct fs_node* inode, unsigned int offset, unsigned int size, unsigned char* buffer);
    unsigned int (*fs_write)(struct fs_node* inode, unsigned int offset, unsigned int size, unsigned char* buffer);
    void (*fs_open)(struct fs_node* inode);
    void (*fs_close)(struct fs_node* inode);
    struct vfs_dirent* (*fs_readdir)(struct fs_node* inode, unsigned int index); //Needed?
    struct fs_node* (*fs_finddir)(struct fs_node* inode, char* name);
    void (*fs_create)(struct fs_node* inode, char* name);
    void (*fs_delete)(struct fs_node* inode);
} fs_operations_t;

typedef struct fs_node
{
    char name[FILE_NAME_MAX];
    unsigned int inode;
    unsigned int size;
    unsigned int flags;
    fs_operations_t* fsops;
    driver_block_t* device;

} fs_node_t;

struct vfs_dirent
{
    char name[FILE_NAME_MAX];
    unsigned int inodenum;
};

typedef struct
{
    char* name;
    fs_operations_t* fops;
    void (*fs_mount)(fs_node_t* mountpoint);

} filesystem_t;

void vfs_initialize();

unsigned int vfs_read(fs_node_t* inode, unsigned int offset, unsigned int size, unsigned char* buffer);
unsigned int vfs_write(fs_node_t* inode, unsigned int offset, unsigned int size, unsigned char* buffer);
void vfs_open(fs_node_t* inode);
void vfs_close(fs_node_t* inode);
struct vfs_dirent* vfs_readdir(fs_node_t* inode, unsigned int index);
fs_node_t* vfs_finddir(fs_node_t*, char* name);
void vfs_create(char* name);
void vfs_delete(char* name);
int vfs_mount(char* device, char* fsname, char* path);


//Maybe separate out in own FS.c/h?
int fs_register(filesystem_t* fs);
filesystem_t* fs_lookup(char* name);


#endif
