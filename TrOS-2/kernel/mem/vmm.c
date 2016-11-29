#include <tros/vmm.h>
#include <tros/pmm.h>
#include <tros/irq.h>
#include <tros/tros.h>
#include <tros/klib/kstring.h>

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

#define PTABLE_ADDR_SPACE_SIZE  0x400000
#define DTABLE_ADDR_SPACE_SIZE  0x100000000

#define _USER_DEBUG 1

pdirectory_t* __current_pdirectory = 0;

extern void paging_flush_tlb_entry(vrt_address addr);
extern void paging_enable(int enable);
extern void paging_load_PDBR(phy_address_t addr);
extern unsigned int paging_get_PDBR();
extern unsigned int paging_error_addr();

void vmm_pagefault_handler(cpu_registers_t* regs);

int vmm_initialize()
{
    ptable_t* table_default = (ptable_t*)pmm_alloc_block();
    if(!table_default)
    {
        return VMM_ERROR_NOMEM;
    }

    ptable_t* table_3_gb = (ptable_t*)pmm_alloc_block();
    if(!table_3_gb)
    {
        return VMM_ERROR_NOMEM;
    }

    vmm_ptable_clear(table_default);
    vmm_ptable_clear(table_3_gb);

    //Identity mapping
    printk("VMM Identitymapped from %x ", 0);
    unsigned int last_addr = 0;
    for(int i=0, block=0x0, virt=0x00000000; i<256; i++, block+=4096, virt+=4096)
    {
       pte_t page = 0;
       pte_add_attribute(&page, PTE_PRESENT);

#ifdef _USER_DEBUG
       pte_add_attribute(&page, PTE_USER);
#endif
       pte_set_block(&page, block);

       table_default->entries[PAGE_TABLE_INDEX(virt)] = page;
       last_addr = virt;
    }
    printk("to %x ", last_addr+4096);
    #ifdef _USER_DEBUG
           printk(" (USER MODE RW+)");
    #endif
    printk("\n");

    for (int i=0, block=0x100000, virt=0xc0000000; i<1024; i++, block+=4096, virt+=4096)
    {
       pte_t page = 0;
       pte_add_attribute(&page, PTE_PRESENT);

#ifdef _USER_DEBUG
       pte_add_attribute(&page, PTE_USER);
#endif
       //TODO: Remove writable and map own temporary kernel stack section as writable
       pte_add_attribute(&page, PTE_WRITABLE); //TO write to stack
       pte_set_block(&page, block);

       table_3_gb->entries[PAGE_TABLE_INDEX(virt)] = page;
    }

    pdirectory_t* dir = (pdirectory_t*)pmm_alloc_block();
    //printk("Dir: %x %x", dir, *dir);
    if (!dir)
    {
        return VMM_ERROR_NOMEM;
    }

    vmm_pdirectory_clear(dir);

    pde_t* entry = &dir->entries[PAGE_DIRECTORY_INDEX(0xc0000000)];
    pde_add_attribute(entry, PDE_PRESENT);
    pde_add_attribute(entry, PDE_WRITABLE);

#ifdef _USER_DEBUG
    pde_add_attribute(entry, PDE_USER);
#endif

    pde_set_pte(entry, (phy_address_t)table_3_gb);

    pde_t* entry2 = &dir->entries[PAGE_DIRECTORY_INDEX(0x00000000)];
    pde_add_attribute(entry2, PDE_PRESENT);
    pde_add_attribute(entry2, PDE_WRITABLE);

#ifdef _USER_DEBUG
    pde_add_attribute(entry2, PDE_USER);
#endif

    pde_set_pte(entry2, (phy_address_t)table_default);

    vmm_switch_pdirectory(dir);

    irq_register_handler(14, vmm_pagefault_handler);
    paging_enable(1);

    return VMM_OK;
}

void vmm_create_and_map(vrt_address virt, unsigned int size, unsigned int flags)
{
    unsigned int blocks = size / VMM_BLOCK_SIZE;
    if(size % VMM_BLOCK_SIZE > 0) blocks++;

    unsigned int endAddr = virt + (VMM_BLOCK_SIZE * blocks);
    for(unsigned int i = virt; i < endAddr; i += VMM_BLOCK_SIZE)
    {
        vmm_map_create_page(i, flags);
    }
}

void vmm_map_create_page(vrt_address virt, unsigned int flags)
{
    //NOTE: If this function needs to be sped up, then dont use the methods,
    //      but use the functionality from the functions without having to set
    //      up and tear down a stackframe every time. Right now I wanted easy
    //      to understand code over fast code.

    void* phys = pmm_alloc_block();

    pdirectory_t* page_directory = vmm_get_directory();
    pde_t* dir = vmm_pdirectory_lookup_entry(page_directory, virt);

    if(!pde_is_present(*dir))
    {
        ptable_t* table = (ptable_t*)pmm_alloc_block();
        if(!table)
        {
            pmm_free_block(phys);
            return;
        }
        else
        {
            vmm_ptable_clear(table);

            pde_t* entry = &page_directory->entries[PAGE_DIRECTORY_INDEX(virt)];

            pde_add_attribute(entry, PDE_PRESENT);
            pde_add_attribute(entry, PDE_WRITABLE);

#ifdef _USER_DEBUG
            pde_add_attribute(entry, PDE_USER);
#else
            if(flags > 0)
            {   //for now we only have this "one" flag that can be set..
                //Add writable as well
                pde_add_attribute(entry, PDE_USER);
            }
#endif

            pde_set_pte(entry, (unsigned int)table);
        }
    }

    ptable_t* table = (ptable_t*)PAGE_GET_PHYSICAL_ADDRESS(dir);
    pte_t* page = &table->entries[PAGE_TABLE_INDEX(virt)];

    //printk("Virt: %x set to phys addr: %x\n", virt, phys);
    //printk("Page at %x, set phys addr: %x\n", page, phys);
    pte_set_block(page, (unsigned int)phys);
    pte_add_attribute(page, PTE_PRESENT);

#ifdef _USER_DEBUG
    pte_add_attribute(page, PTE_USER);
    pte_add_attribute(page, PTE_WRITABLE);
#endif
}

// int vmm_alloc_page(pte_t* page)
// {
//     void* block_addr = pmm_alloc_block();
//     if(!block_addr)
//     {
//         return -1;
//     }
//     else
//     {
//         pte_set_block(page, (unsigned int)block_addr);
//         pte_add_attribute(page, PTE_PRESENT);
//         return 1;
//     }
// }
//
// void vmm_free_page(pte_t* page)
// {
//     void* block_addr = (void*)pte_get_block_addr(*page);
//     if(block_addr)
//     {
//         pmm_free_block(block_addr);
//     }
//     pte_delete_attribute(page, PTE_PRESENT);
// }

int vmm_switch_pdirectory(pdirectory_t* dir)
{
    if (!dir)
    {
        return -1;
    }
    else
    {
        __current_pdirectory = dir;
        paging_load_PDBR((phy_address_t)&dir->entries);
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

/***
    Clones a page directory.
    User mode pages are cloned, kernel pages are "linked"
**/
pdirectory_t * vmm_clone_directory(pdirectory_t* src)
{
    //TODO!
    // - Link the kernel pages
    // - Link the Identitymapped <1Mb region
    // - Empty / clear out rest

    return __current_pdirectory;
}

void vmm_pagefault_handler(cpu_registers_t* regs)
{
    unsigned int faulting_address = paging_error_addr();


    int not_present = !(regs->err_code & 0x1);
    int write_operation = regs->err_code & 0x2;
    int user_mode = regs->err_code & 0x4;
    int cpu_reserved = regs->err_code & 0x8;


    printk("Page fault! ( ");
    if(not_present)
    {
        printk("not-present ");
    }
    if(write_operation)
    {
        printk("read-only ");
    }
    if(user_mode)
    {
        printk("user-mode ");
    }
    if(cpu_reserved)
    {
        printk("reserved ");
    }
    printk(") at %x\n", faulting_address);

    kernel_panic("Page fault", regs);

}
