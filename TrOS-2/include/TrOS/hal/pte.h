// pte.h
// Page Table Entry datastructures

#ifndef INCLUDE_TROS_PTE_H
#define INCLUDE_TROS_PTE_H

enum PTE_PAGE_FLAGS
{
    PTE_PRESENT         = 0x01,
    PTE_WRITABLE        = 0x02,
    PTE_USER            = 0x04,
    PTE_WRITETHOUGH     = 0x08,
    PTE_NOT_CACHEABLE   = 0x10,
    PTE_ACCESSED        = 0x20,
    PTE_DIRTY           = 0x40,
    PTE_PAT             = 0x80,
    PTE_CPU_GLOBAL      = 0x100,
    PTE_LV4_GLOBAL      = 0x200,
    PTE_BLOCK           = 0x7FFFF000
};

typedef unsigned int pte_t;

void pte_add_attribute(pte_t* e, unsigned int attrib);
void pte_delete_attribute(pte_t* e, unsigned int attrib);
void pte_set_block(pte_t* e, unsigned int addr);
int pte_is_present(pte_t e);
int pte_is_writable(pte_t e);
unsigned int pte_get_block_addr(pte_t e);


#endif
