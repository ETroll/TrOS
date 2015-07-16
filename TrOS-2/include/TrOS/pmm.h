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

void pmm_initialize(unsigned int addr, unsigned int size);

//Enable a physical region for use with the PMM
void pmm_init_region(unsigned int addr, unsigned int size);

//Disable a phyical region for use with the PMM
void pmm_deinit_region(unsigned int addr, unsigned int size);
void* pmm_alloc_block();
void pmm_free_block(void* blockptr);
void* pmm_alloc_blocks(unsigned int size);
void pmm_free_blocks(void* blockptr, unsigned int size);
unsigned int pmm_get_memory_size();
unsigned int pmm_get_use_block_count();
unsigned int pmm_get_free_block_count();
unsigned int pmm_get_block_count();
unsigned int pmm_get_block_size();
void pmm_paging_enable(int enable);
int pmm_is_paging_enabled();
void pmm_load_PDBR(unsigned int physical_addr);
unsigned int pmmngr_get_PDBR();

#endif
