
#ifndef INCLUDE_TROS_VMMV2_H
#define INCLUDE_TROS_VMMV2_H

#include <stdint.h>

#define VMM2_BLOCK_SIZE 0x00001000

typedef enum
{
    VMM2_ERROR = -1,
    VMM2_OK = 1
} vmm2_status_t;

typedef unsigned int virtual_addr_t;
// typedef unsigned int page_t;
// typedef unsigned int page_table_t;

typedef struct {
    unsigned int present:1;
    unsigned int writable:1;
    unsigned int user:1;
    unsigned int writethrough:1;
    unsigned int cachedisable:1;
    unsigned int unused:7;          // These bits differ from table entry
    unsigned int frame:20;          // and directory entry.
} __attribute__((packed)) page_entry_t;

enum vmm2_page_flags
{
    VMM2_PAGE_NOFLAG = 0,
    VMM2_PAGE_PRESENT = 1,
    VMM2_PAGE_WRITABLE = 2,
    VMM2_PAGE_USER = 4
};

typedef struct
{
    uint32_t entries[1024];
} page_table_t;

typedef struct
{
    uint32_t tables[1024];
} page_directory_t;


vmm2_status_t vmm2_initialize(uint32_t stack, uint32_t size, uint32_t regionMapLocation);
void vmm2_identitymap(uint32_t phys, uint32_t blocks, uint32_t flags);
void vmm2_map(virtual_addr_t virt, uint32_t blocks, uint32_t flags);
void vmm2_physicalmap(uint32_t phys, virtual_addr_t virt, uint32_t blocks, uint32_t flags);

void vmm2_identitymap_todir(uint32_t phys, uint32_t blocks, uint32_t flags, page_directory_t* dir);
void vmm2_physicalmap_todir(uint32_t phys, virtual_addr_t virt, uint32_t blocks, uint32_t flags, page_directory_t* dir);
void vmm2_map_todir(virtual_addr_t virt, uint32_t blocks, uint32_t flags, page_directory_t* dir);

vmm2_status_t vmm2_switch_pagedir(page_directory_t* dir);

page_directory_t* vmm2_get_directory();
uint32_t* vmm2_get_pagetable(virtual_addr_t virt, page_directory_t* dir, uint32_t create);
// page_directory_t* vmm2_clone_directory(page_directory_t* src);
page_directory_t* vmm2_create_directory();

#endif
