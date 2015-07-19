#include <tros/hal/pte.h>

void pte_add_attribute(pte_t* e, unsigned int attrib)
{
    *e |= attrib;
}

void pte_delete_attribute(pte_t* e, unsigned int attrib)
{
    *e &= ~attrib;
}

void pte_set_block(pte_t* e, unsigned int addr)
{
    *e = (*e & ~PTE_BLOCK) | addr;
}

int pte_is_present(pte_t e)
{
    return e & PTE_PRESENT;
}
int pte_is_writable(pte_t e)
{
    return e & PTE_WRITABLE;
}

unsigned int pte_get_block_addr(pte_t e)
{
    return e & PTE_BLOCK;
}
