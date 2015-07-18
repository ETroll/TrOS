#include <tros/vmm.h>
#include <tros/pmm.h>
#include <string.h>

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

#define PTABLE_ADDR_SPACE_SIZE  0x400000
#define DTABLE_ADDR_SPACE_SIZE  0x100000000
#define PAGE_SIZE               4096

pdirectory_t* __current_pdirectory = 0;
unsigned int __current_pdbr_addr = 0;

extern void paging_flush_tlb_entry(vrt_address addr);
extern void paging_enable(int enable);
extern void paging_load_PDBR(phy_address addr);
extern unsigned int paging_get_PDBR();

void vmm_initialize()
{
    ptable_t* table_default = (ptable_t*)pmm_alloc_block();
    if(!table_default)
    {
        return;
    }

    ptable_t* table_3_gb = (ptable_t*)pmm_alloc_block();
    if(!table_3_gb)
    {
        return;
    }

    vmm_ptable_clear(table_default);
    vmm_ptable_clear(table_3_gb);

    //Identity mapping
    for(int i=0, block=0x0, virt=0x00000000; i<1024; i++, block+=4096, virt+=4096)
    {
       pte_t page = 0;
       pte_add_attribute(&page, PTE_PRESENT);
       pte_set_block(&page, block);

       table_default->entries[PAGE_TABLE_INDEX(virt)] = page;
    }

    for (int i=0, block=0x100000, virt=0xc0000000; i<1024; i++, block+=4096, virt+=4096)
    {
       pte_t page = 0;
       pte_add_attribute(&page, PTE_PRESENT);
       pte_set_block(&page, block);

       table_3_gb->entries[PAGE_TABLE_INDEX(virt)] = page;
    }

    pdirectory_t* dir = (pdirectory_t*)pmm_alloc_blocks(3);
    if (!dir)
    {
        return;
    }

    vmm_pdirectory_clear(dir);

    pde_t* entry = &dir->entries[PAGE_DIRECTORY_INDEX(0xc0000000)];
    pde_add_attribute(entry, PDE_PRESENT);
    pde_add_attribute(entry, PDE_WRITABLE);
    pde_set_pte(entry, (phy_address)table_3_gb);

    pde_t* entry2 = &dir->entries[PAGE_DIRECTORY_INDEX(0x00000000)];
    pde_add_attribute(entry2, PDE_PRESENT);
    pde_add_attribute(entry2, PDE_WRITABLE);
    pde_set_pte(entry2, (phy_address)table_default);

    __current_pdbr_addr = (phy_address)&dir->entries;

    //TODO: Make the updating of pdbr variable happen in vmm_switch_pdirectory
    vmm_switch_pdirectory(dir);
    paging_enable(1);

}

void vmm_map_page(void* phys, void* virt)
{
    //NOTE: If this function needs to be sped up, then dont use the methods,
    //      but use the functionality from the functions without having to set
    //      up and tear down a stackframe every time. Right now I wanted easy
    //      to understand code over fast code.

    pdirectory_t* page_directory = vmm_get_directory();
    pde_t* dir = vmm_pdirectory_lookup_entry(page_directory, (vrt_address)virt);

    if(!pde_is_present(*dir))
    {
        ptable_t* table = (ptable_t*)pmm_alloc_block();
        if(!table)
        {
            return;
        }
        else
        {
            vmm_ptable_clear(table);

            pde_t* entry = &page_directory->entries[PAGE_DIRECTORY_INDEX((vrt_address)virt)];

            pde_add_attribute(entry, PDE_PRESENT);
            pde_add_attribute(entry, PDE_WRITABLE);
            pde_set_pte(entry, (unsigned int)table);
        }
    }

    ptable_t* table = (ptable_t*)PAGE_GET_PHYSICAL_ADDRESS(dir);
    pte_t* page = &table->entries[PAGE_TABLE_INDEX((vrt_address)virt)];

    pte_set_block(page, (unsigned int)phys);
    pte_add_attribute(page, PTE_PRESENT);
}

int vmm_alloc_page(pte_t* page)
{
    void* block_addr = pmm_alloc_block();
    if(!block_addr)
    {
        return -1;
    }
    else
    {
        pte_set_block(page, (unsigned int)block_addr);
        pte_add_attribute(page, PTE_PRESENT);
        return 1;
    }
}

void vmm_free_page(pte_t* page)
{
    void* block_addr = (void*)pte_get_block_addr(*page);
    if(block_addr)
    {
        pmm_free_block(block_addr);
    }
    pte_delete_attribute(page, PTE_PRESENT);
}

int vmm_switch_pdirectory (pdirectory_t* dir)
{
    if (!dir)
    {
        return -1;
    }
    else
    {
        __current_pdirectory = dir;
        paging_load_PDBR(__current_pdbr_addr);
        return 1;
    }
}

pdirectory_t* vmm_get_directory()
{
    return __current_pdirectory;
}

void vmm_ptable_clear(ptable_t* table)
{
    memset(table, 0, sizeof(ptable_t));
}

unsigned int vmm_ptable_virt_to_index(vrt_address addr)
{
    return PAGE_TABLE_INDEX(addr);
}

pte_t* vmm_ptable_lookup_entry(ptable_t* page, vrt_address addr)
{
    if (page)
    {
        return &page->entries[PAGE_TABLE_INDEX(addr)];
    }
    else
    {
        return 0;
    }
}

unsigned int vmm_pdirectory_virt_to_index(vrt_address addr)
{
    return PAGE_DIRECTORY_INDEX(addr);
}

void vmm_pdirectory_clear(pdirectory_t* dir)
{
    memset(dir, 0, sizeof(pdirectory_t));
}

pde_t* vmm_pdirectory_lookup_entry(pdirectory_t* dir, vrt_address addr)
{
    if (dir)
    {
        return &dir->entries[PAGE_DIRECTORY_INDEX(addr)];
    }
    else
    {
        return 0;
    }
}
