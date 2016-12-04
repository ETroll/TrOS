// pmm.h
// Physical memory manager

#ifndef INCLUDE_TROS_PMM_H
#define INCLUDE_TROS_PMM_H

typedef unsigned int physical_addr_t;

int pmm_initialize(physical_addr_t addr, unsigned int size, physical_addr_t regions);

//Enable a physical region for use with the PMM
void pmm_init_region(physical_addr_t addr, unsigned int size);

//Disable a phyical region for use with the PMM
void pmm_deinit_region(physical_addr_t addr, unsigned int size);
void pmm_deinit_block(physical_addr_t addr);
void* pmm_alloc_block();
void pmm_free_block(void* blockptr);
void* pmm_alloc_blocks(unsigned int size);
void pmm_free_blocks(void* blockptr, unsigned int size);
unsigned int pmm_get_memory_size();
unsigned int pmm_get_use_block_count();
unsigned int pmm_get_free_block_count();
unsigned int pmm_get_block_count();
unsigned int pmm_get_block_size();

#endif
