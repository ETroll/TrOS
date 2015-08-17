// pmm.h
// Physical memory manager

#ifndef INCLUDE_TROS_PMM_H
#define INCLUDE_TROS_PMM_H

typedef struct {
    unsigned int startLo;
    unsigned int startHi;
    unsigned int sizeLo;
    unsigned int sizeHi;
    unsigned int type;
    unsigned int acpi_3_0;
} pmm_region_t;

typedef unsigned int phy_address;

int pmm_initialize(unsigned int addr, unsigned int size, pmm_region_t* regions);

//Enable a physical region for use with the PMM
void pmm_init_region(phy_address addr, unsigned int size);

//Disable a phyical region for use with the PMM
void pmm_deinit_region(phy_address addr, unsigned int size);
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
