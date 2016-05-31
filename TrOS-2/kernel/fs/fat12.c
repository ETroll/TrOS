// fat12.c
// A FAT12 "minidriver". Basically just a practicerun...

#include <tros/fs/vfs.h>
#include <tros/fs.h>
#include <tros/tros.h>
#include <tros/kheap.h>

#include <string.h>

#define FAT12_DIR_MAX 16
#define FAT12_BPS 512
#define FAT12_SECT_OFFSET 33

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

typedef struct
{
    char filename[8];
    char extension[3];
    unsigned char attributes;
    unsigned short reserved;
    unsigned short createdtime;
    unsigned short createddate;
    unsigned short lastaccess;
    unsigned short ignore;
    unsigned short writetime;
    unsigned short writedate;
    unsigned short firstcluster;
    unsigned int size;
} __attribute__ ((packed)) fat12_entry_t;

enum FAT12_ATTRIBUTE
{
    FAT12_ATR_READONLY  = 0x01,
    FAT12_ATR_HIDDEN    = 0x02,
    FAT12_ATR_SYSTEM    = 0x04,
    FAT12_ATR_VOLBL     = 0x08,
    FAT12_ATR_DIRECTORY = 0x10,
    FAT12_ATR_FILE      = 0x20,
};

static unsigned short fat12_table_entry(unsigned short sector, unsigned char* table);

static int fat12_mount(fs_node_t* mountpoint);
static unsigned int fat12_read(struct fs_node* node, unsigned int offset,
                                unsigned int size, unsigned char* buffer);
static unsigned int fat12_write(struct fs_node* node, unsigned int offset,
                                unsigned int size, unsigned char* buffer);
static void fat12_open(struct fs_node* node);
static void fat12_close(struct fs_node* node);
static dirent_t* fat12_readdir(fs_node_t* node, unsigned int index);

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
        .mount = fat12_mount
    };
    return fs_register(&fs);
}

int fat12_mount(fs_node_t* mountpoint)
{
    printk("Trying to mount %s as FAT12\n", mountpoint->name);

    //unsigned char* buffer = (unsigned char*)0x1000;
    fat12_super_t* super = (fat12_super_t*)kmalloc(FAT12_BPS);

    if(mountpoint->device->open())
    {
        if(mountpoint->device->read((unsigned char*)super, 0, 1) > 0)
        {
            printk("OEM: %s - Sectors: %d - FATs: %d - Rootdir: %d (%d sects)\n",
                super->oem,
                super->total_sects,
                super->num_fats,
                super->rootdir_entries,
                super->rootdir_entries / FAT12_DIR_MAX
            );
            mountpoint->inode = (super->num_fats * super->sects_fat) + 1;
            mountpoint->size = 0;
        }

        // Test - Proof of concept
        // printk("Starting with rootdir at: %d (%x)\n", mountpoint->inode);
        // fat12_entry_t* rootdir = (fat12_entry_t*)kmalloc(super->bps);
        // mountpoint->device->read((unsigned char*)rootdir, mountpoint->inode, 1);
        //
        // for(int i = 0; i<16 && rootdir[i].filename[0] != 0; i++)
        // {
        //     printk("%s %s - %x - %i - %ib\n",
        //         rootdir[i].filename,
        //         rootdir[i].extension,
        //         rootdir[i].attributes,
        //         rootdir[i].firstcluster,
        //         rootdir[i].size);
        // }
        // kfree(rootdir);
        // End test
        mountpoint->device->close();
        kfree(super);
    }

    return 1;
}

static unsigned int fat12_read(struct fs_node* node, unsigned int offset,
                                unsigned int size, unsigned char* buffer)
{
    //Look up FAT entry for
    //0. Read Super for BPS / 512;
    //1. Calculate start sector based on offset
    //unsigned int sector = node->inode + (offset / FAT12_BPS);

    unsigned int total_bytes_read = 0;

    fat12_super_t* super = (fat12_super_t*)kmalloc(FAT12_BPS);
    if(node->device->read((unsigned char*)super, 0, 1) > 0)
    {
        unsigned char* fat_table = (unsigned char*)kmalloc(FAT12_BPS * super->sects_fat);
        if(node->device->read((unsigned char*)fat_table, 1, super->sects_fat) > 0)
        {
            unsigned short sector = node->inode + (offset / FAT12_BPS);
            unsigned char* readbuffer = (unsigned char*)kmalloc(FAT12_BPS);
            unsigned int start_offset = offset;

            do
            {
                unsigned int bytes_to_read = 0;
                memset(readbuffer, '\0', FAT12_BPS); //Should I clear or kfree/kmalloc?
                if(node->device->read(readbuffer, sector, 1) > 0)
                {
                    if(start_offset > 0)
                    {
                        if(start_offset + size < FAT12_BPS)
                        {
                            bytes_to_read = size;
                        }
                        else
                        {
                            bytes_to_read = (FAT12_BPS-start_offset);
                        }
                        //bit of a special case here, if we are exec this then we have not read
                        //anything before.. so safe to not use a offset for the buffer
                        memcpy(buffer, (readbuffer+start_offset), bytes_to_read);
                        start_offset = 0;
                    }
                    else
                    {
                        if((size-total_bytes_read) > FAT12_BPS)
                        {
                            bytes_to_read = FAT12_BPS;
                        }
                        else
                        {
                            bytes_to_read = (size-total_bytes_read);
                        }
                        memcpy((buffer+total_bytes_read), readbuffer, bytes_to_read);
                    }
                    total_bytes_read += bytes_to_read;

                    //get next sector from FAT
                    sector = fat12_table_entry(sector, fat_table);
                }
                else
                {
                    //ABORT!
                    break;
                }
            }
            while((size-total_bytes_read) > 0  && sector != 0x00 && sector < 0x0FF0);
            kfree(readbuffer);
        }
        kfree(fat_table);
    }
    kfree(super);

    return total_bytes_read;
}

static unsigned int fat12_write(struct fs_node* node, unsigned int offset,
                                unsigned int size, unsigned char* buffer)
{
    return 0;
}
/*
 * Basically the inode is a directory inode, and the index is the file number.
 * So we return the name and inode (addr) of the file at index
 */
static dirent_t* fat12_readdir(fs_node_t* node, unsigned int index)
{
    //printk("Node: %x, inode: %d device: %x index: %d bps: %x\n", node, node->inode, node->device, index, FAT12_BPS);
    fat12_entry_t* directory = (fat12_entry_t*)kmalloc(FAT12_BPS);
    int di = index % FAT12_DIR_MAX;
    int sectors_read = 0;
    if(index < FAT12_DIR_MAX)
    {
        //printk("Read dir: %x, inode %x device: %x read %x\n", directory, node->inode,  node->device->read);
        sectors_read = node->device->read((unsigned char*)directory, node->inode, 1);
    }
    else
    {
        // Look up in FAT table or (Or if inode < 32, then increment to next)
        // And load data into directory entry
        printk("ERROR! Not implemented index > 16!\n");
    }

    if(sectors_read > 0)
    {
        
        //printk("di: %d filn[0]: %x\n", di, directory[di].filename[0]);
        if(directory[di].filename[0] != 0x00 && directory[di].filename[0] != 0xE5)
        {
            dirent_t* entry = (dirent_t*)kmalloc(sizeof(dirent_t));
            strncpy(entry->name, directory[di].filename, 8);
            strncpy((char*)&entry->name[8], directory[di].extension, 3);
            entry->name[11] = '\0';

            entry->inodenum = directory[di].firstcluster + FAT12_SECT_OFFSET;
            entry->flags = 0;

            if((directory[di].attributes & FAT12_ATR_DIRECTORY) == FAT12_ATR_DIRECTORY)
            {
                entry->flags |= VFS_FLAG_DIRECTORY;
            }
            if((directory[di].attributes & FAT12_ATR_FILE) == FAT12_ATR_FILE)
            {
                entry->flags |= VFS_FLAG_FILE;
            }
            return entry;
        }
        else
        {
            //TODO: Handle edgecase 0xE5 where entry is free, but there are
            //      More entries left
            return 0;
        }
    }
    else
    {
        printk("ERROR! No sectors read!\n");
    }

    kfree(directory);

    return 0;
}

static void fat12_open(struct fs_node* node)
{
    node->device->open();
}

static void fat12_close(struct fs_node* node)
{
    node->device->close();
}

static unsigned short fat12_table_entry(unsigned short sector, unsigned char* table)
{
    unsigned int translated = (sector-FAT12_SECT_OFFSET) + 2;
    unsigned short value = *(unsigned short*)&table[translated];

    if(sector & 0x0001)
    {
        value = value >> 4;
    }
    else
    {
        value = value & 0x0FFF;
    }
    return value;
}
