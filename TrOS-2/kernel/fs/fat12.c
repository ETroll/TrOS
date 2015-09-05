// fat12.c
// A FAT12 "minidriver". Basically just a practicerun...

#include <tros/fs/vfs.h>
#include <tros/tros.h>
#include <tros/kheap.h>

#include <string.h>

typedef struct
{
    unsigned char bootstrap[3];
    unsigned char oem[8];
    unsigned short bps;             //Bytes per sector
    unsigned char spc;              //Sectors per cluster
    unsigned short num_rs;          //Reserved sectors
    unsigned char num_fats;         //Number of FAT copies;
    unsigned short rootdir_entries; //Number of entries in the root directory
    unsigned short total_sects;     //Total number of sectors on disk
    unsigned char media;            //Type of media
    unsigned short sects_fat;       //Sectors per FAT
    unsigned short sects_track;     //Sectors per track
    unsigned short num_heads;       //NUmber of heads
    unsigned short num_hidden;      //Number of hidden sectors


} __attribute__ ((packed)) fat12_super_t;


int fat12_mount(fs_node_t* mountpoint);

static unsigned int fat12_read(struct fs_node* inode, unsigned int offset,
                                unsigned int size, unsigned char* buffer);
static unsigned int fat12_write(struct fs_node* inode, unsigned int offset,
                                unsigned int size, unsigned char* buffer);
static void fat12_open(struct fs_node* inode);
static void fat12_close(struct fs_node* inode);
static dirent_t* fat12_readdir(fs_node_t* inode, unsigned int index);

static fs_operations_t fsops =
{
    .fs_read = fat12_read,
    .fs_write = fat12_write,
    .fs_open = fat12_open,
    .fs_close = fat12_close,
    .fs_readdir = fat12_readdir,
    .fs_create = 0,
    .fs_delete = 0
};

int fat12_fs_initialize()
{
    filesystem_t fs =
    {
        .name = "fat12",
        .fsops = &fsops,
        .fs_mount = fat12_mount
    };
    return fs_register(&fs);
}

int fat12_mount(fs_node_t* mountpoint)
{
    printk("Trying to mount %s as FAT12\n", mountpoint->name);

    //unsigned char* buffer = (unsigned char*)0x1000;
    fat12_super_t* super = (fat12_super_t*)kmalloc(512);

    if(mountpoint->device->open())
    {
        if(mountpoint->device->read((unsigned char*)super, 0, 1) > 0)
        {
            printk("OEM: %s - Sectors: %d - FATs: %d - Rootdir: %d\n",
                super->oem,
                super->total_sects,
                super->num_fats,
                super->rootdir_entries);
            mountpoint->inode = (super->num_fats * super->sects_fat) + 1;
            mountpoint->size = 0;
        }
        mountpoint->device->close();
    }

    return 1;
}

static unsigned int fat12_read(struct fs_node* inode, unsigned int offset,
                                unsigned int size, unsigned char* buffer)
{
    return 0;
}

static unsigned int fat12_write(struct fs_node* inode, unsigned int offset,
                                unsigned int size, unsigned char* buffer)
{
    return 0;
}
/*
 * Basically the inode is a directory inode, and the index is the file number.
 * So we return the name and inode (addr) of the file at index
 */
static dirent_t* fat12_readdir(fs_node_t* inode, unsigned int index)
{
    if(index < 3)
    {
        dirent_t* dir = (dirent_t*)kmalloc(sizeof(dirent_t));
        strcpy(dir->name, "File");
        dir->name[4] = 0x30 + index;
        dir->name[5] = '\0';
        dir->inodenum = 1;

        return dir;
    }
    else
    {
        return 0;
    }
}

static void fat12_open(struct fs_node* inode)
{

}

static void fat12_close(struct fs_node* inode)
{

}
