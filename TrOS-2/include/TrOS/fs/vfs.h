// vfs.h
// Virtual File System

#ifndef INCLUDE_TROS_VFS_H
#define INCLUDE_TROS_VFS_H

#include <tros/driver.h>

#define FILE_NAME_MAX 256
#define PATH_DELIMITER '/'

struct fs_node;
struct fs_dirent;

enum VFS_FLAGS
{
    VFS_FLAG_UNKNOWN    = 0x00,
    VFS_FLAG_FILE       = 0x01,
    VFS_FLAG_DIRECTORY  = 0x02,
    VFS_FLAG_MOUNTPOINT = 0x04,
    VFS_FLAG_ROOTDIR    = 0x08
};

typedef struct
{
    unsigned int (*fs_read)(struct fs_node* inode, unsigned int offset, unsigned int size, unsigned char* buffer);
    unsigned int (*fs_write)(struct fs_node* inode, unsigned int offset, unsigned int size, unsigned char* buffer);
    void (*fs_open)(struct fs_node* inode);
    void (*fs_close)(struct fs_node* inode);
    struct fs_dirent* (*fs_readdir)(struct fs_node* inode, unsigned int index);
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

typedef struct fs_dirent
{
    char name[FILE_NAME_MAX];
    unsigned int inodenum;
    unsigned char flags;
} dirent_t;

void vfs_initialize();

unsigned int vfs_read(fs_node_t* inode, unsigned int offset, unsigned int size, unsigned char* buffer);
unsigned int vfs_write(fs_node_t* inode, unsigned int offset, unsigned int size, unsigned char* buffer);

void vfs_open(fs_node_t* inode);
void vfs_close(fs_node_t* inode);

void vfs_create(char* name);
void vfs_delete(char* name);

dirent_t* vfs_readdir(fs_node_t* inode, unsigned int index);

int vfs_mount(char* device, char* fsname);

fs_node_t* kopen(char* path);

#endif
