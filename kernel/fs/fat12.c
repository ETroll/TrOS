// fat12.c
// A FAT12 "minidriver". Basically just a practicerun...

#include <tros/fs/vfs.h>
#include <tros/fs.h>
#include <tros/tros.h>
#include <tros/memory.h>

#include <tros/klib/kstring.h>

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
    FAT12_ATR_VOLLBL    = 0x08,
    FAT12_ATR_DIRECTORY = 0x10,
    FAT12_ATR_FILE      = 0x00,
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
        mountpoint->device->close();
        kfree(super);
    }

    return 1;
}

static unsigned int fat12_read(struct fs_node* node, unsigned int offset,
                                unsigned int size, unsigned char* buffer)
{
    unsigned int total_bytes_read = 0;
    // printk("F");
    fat12_super_t* super = (fat12_super_t*)kmalloc(FAT12_BPS);
    // printk("12 ");
    if(node->device->read((unsigned char*)super, 0, 1) > 0)
    {
        unsigned char* fat_table = (unsigned char*)kmalloc(FAT12_BPS * super->sects_fat);
        if(node->device->read((unsigned char*)fat_table, 1, super->sects_fat) > 0)
        {
            unsigned short sector = node->inode;
            unsigned int sectorOffset = (offset / FAT12_BPS);
            for(int i = 0; i<sectorOffset; i++)
            {
                // printk("Sect: %d -> ", sector);
                sector = fat12_table_entry(sector, fat_table);
                // printk("Sect: %d ", sector);
            }

            unsigned char* readbuffer = (unsigned char*)kmalloc(FAT12_BPS);
            unsigned int start_offset = (offset % FAT12_BPS);
            do
            {
                // printk("R %d(%d) ",size-total_bytes_read, sector);
                unsigned int bytes_to_read = 0;
                memset(readbuffer, '\0', FAT12_BPS);
                if(node->device->read(readbuffer, sector, 1) > 0)
                {
                    // printk("K ");
                    if(start_offset > 0)
                    {
                        // printk("O ");
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
                        // printk("S ");
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
                    // printk("TB %d ", total_bytes_read);
                    //get next sector from FAT
                    sector = fat12_table_entry(sector, fat_table);
                }
                else
                {
                    //ABORT!
                    printk("ABORT Error reading from sector ");
                    break;
                }
            }
            while((size-total_bytes_read) > 0  && sector != 0x00 && sector < 0x0FF0);
            // printk("\n");
            kfree(readbuffer);
        } else { printk("FAT12: Error reading FAT table\n"); }
        kfree(fat_table);
    } else { printk("FAT12: Error reading super\n"); }
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
        //printk("Read dir: %x, inode %d\n", directory, node->inode,  node->device->read);
        sectors_read = node->device->read((unsigned char*)directory, node->inode, 1);
    }
    else
    {
        // Look up in FAT table or (Or if inode < 32, then increment to next)
        // And load data into directory entry
        //TODO: Implement support for larger folders than 16
        printk("ERROR! Not implemented index > 16!\n");
    }

    if(sectors_read > 0)
    {

        //printk("%d file: %s size %d cluster %d creation %d\n", di, directory[di].filename, directory[di].size, directory[di].firstcluster, directory[di].createdtime);
        if(directory[di].filename[0] != 0x00 && directory[di].filename[0] != 0xE5)
        {
            dirent_t* entry = (dirent_t*)kmalloc(sizeof(dirent_t));
            char tmpFilename[9];
            char tmpExtension[4];

            strncpy(tmpFilename, directory[di].filename, 8);
            tmpFilename[8] = '\0';
            strncpy(tmpExtension, directory[di].extension, 3);
            tmpExtension[3] = '\0';
            trimend(tmpFilename);
            trimend(tmpExtension);

            entry->inodenum = directory[di].firstcluster + (FAT12_SECT_OFFSET-2);
            entry->flags = VFS_FLAG_FILE;
            entry->size = directory[di].size;

            if((directory[di].attributes & FAT12_ATR_DIRECTORY) == FAT12_ATR_DIRECTORY)
            {
                entry->flags |= VFS_FLAG_DIRECTORY;
            }

            strcpy(entry->name, tmpFilename);
            if(strlen(tmpExtension)> 0)
            {
                unsigned int namelength = strlen(tmpFilename);
                entry->name[namelength++] = '.';
                strcpy((char*)&entry->name[namelength], tmpExtension);
            }

            //entry->name[strlen(entry->name)] = '\0';

            //printk("|%s| and |%s| makes: |%s| \n",tmpFilename, tmpExtension, entry->name);
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
    //unsigned int translated = (sector-FAT12_SECT_OFFSET) + 2;
    //printk("ST: %d/%d ", sector, translated);
    unsigned int translated = sector * 1.5;
    //printk("T: %d ", translated);

    unsigned short value = *(unsigned short*)&table[translated];
    //printk("V: %x ", value);

    if(sector & 0x0001)
    {
        value = value >> 4;
    }
    else
    {
        value = value & 0x0FFF;
    }
    //printk("FTL: %d ", value);
    return value;
}
