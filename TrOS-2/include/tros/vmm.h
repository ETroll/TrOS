// vmm.h
// Virtual Memory Manager

#ifndef INCLUDE_TROS_VMM_H
#define INCLUDE_TROS_VMM_H

#include <tros/hal/pte.h>
#include <tros/hal/pde.h>

#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR   1024

#define VMM_ERROR_NOMEM -1
#define VMM_OK          1

#define VMM_BLOCK_SIZE      4096
#define VMM_BLOCK_ALIGN     VMM_BLOCK_SIZE

#define VMM_FLAG_USER   1
#define VMM_FLAG_KERNEL 0

typedef unsigned int vrt_address;

typedef struct
{
    pte_t entries[PAGES_PER_TABLE];
} ptable_t;

//! page directory
typedef struct
{
    pde_t entries[PAGES_PER_DIR];
} pdirectory_t;


int vmm_initialize();
void vmm_create_and_map(vrt_address virt, unsigned int size, unsigned int flags);
void vmm_map_create_page(vrt_address virt, unsigned int flags);

// int vmm_alloc_page(pte_t* page);
// void vmm_free_page(pte_t* page);

int vmm_switch_pdirectory (pdirectory_t* dir);
pdirectory_t* vmm_get_directory();

void vmm_ptable_clear(ptable_t* p);
unsigned int vmm_ptable_virt_to_index(vrt_address addr);
pte_t* vmm_ptable_lookup_entry(ptable_t* p, vrt_address addr);

unsigned int vmm_pdirectory_virt_to_index(vrt_address addr);
void vmm_pdirectory_clear(pdirectory_t* dir);
pde_t* vmm_pdirectory_lookup_entry(pdirectory_t* p, vrt_address addr);

pdirectory_t * vmm_clone_directory(pdirectory_t* src);

#endif
