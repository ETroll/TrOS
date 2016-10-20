// pde.h
// Page Directory Entries datastructures and methods

#ifndef INCLUDE_TROS_PDE_H
#define INCLUDE_TROS_PDE_H

enum PDE_PAGE_FLAGS
{
    PDE_PRESENT         = 0x01,
    PDE_WRITABLE        = 0x02,
    PDE_USER            = 0x04,
    PDE_PWT             = 0x08,
    PDE_PCD             = 0x10,
    PDE_ACCESSED        = 0x20,
    PDE_DIRTY           = 0x40,
    PDE_4MB             = 0x80,
    PDE_CPU_GLOBAL      = 0x100,
    PDE_LV4_GLOBAL      = 0x200,
    PDE_PAGE_TABLE      = 0x7FFFF000
};

typedef unsigned int pde_t;

void pde_add_attribute(pde_t* e, unsigned int attrib);
void pde_delete_attribute(pde_t* e, unsigned int attrib);
void pde_set_pte(pde_t* e, unsigned int addr);
int pde_is_present(pde_t e);
int pde_is_user(pde_t e);
int pde_is_4mb(pde_t e);
int pde_is_writable(pde_t e);
unsigned int pde_get_pte_addr(pde_t e);


#endif
