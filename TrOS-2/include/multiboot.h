// multiboot.h
// Handling and structures for multiboot support

#ifndef INCLUDE_MULTIBOOT_H
#define INCLUDE_MULTIBOOT_H

typedef struct  {
    unsigned int flags;
    unsigned int memoryLo;
    unsigned int memoryHi;
    unsigned int bootDevice;
    unsigned int cmdLine;
    unsigned int modsCount;
    unsigned int modsAddr;
    unsigned int syms0;
    unsigned int syms1;
    unsigned int syms2;
    unsigned int mmap_length;
    unsigned int mmap_addr;
    unsigned int drives_length;
    unsigned int drives_addr;
    unsigned int config_table;
    unsigned int bootloader_name;
    unsigned int aptable;
    unsigned int vbe_control_info;
    unsigned int vbe_mode_info;
    unsigned short vbe_mode;
    unsigned int vbe_interface_addr;
    unsigned short vbe_interface_len;
} multiboot_info_t;


#endif
