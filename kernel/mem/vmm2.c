#include <tros/mem/vmm2.h>
#include <tros/mem/pmm.h>
#include <tros/irq.h>
#include <tros/tros.h>
#include <tros/klib/kstring.h>

// #define _USER_DEBUG 1 //Just a debug for now with flags

//Virtual Address: 0x08048001
//0000 1000 00|00 0100 1000|0000 0000 0001
// 0    8    0     4    8    0    0    1
//|-----------|------------|-------------|
//    10bit        10bit       12bit
//    0x3FF        0x3FF       0xFFF
//  PDir Index  Table Index   Phys Off (4K)

#define DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define ENTRY_PHYS_ADDRESS(x) (*x & ~0xfff)

#define KERNEL_PHYSICAL 0x00100000
#define KERNEL_VIRTUAL  0xC0000000

static page_directory_t* _kernel_dir = 0;
// static page_directory_t* _current_dir = 0;

extern int paging_is_enabled();
extern void paging_enable(int enable);
extern void paging_set_CR3(uint32_t phys);
extern page_directory_t* paging_get_CR3();

void vmm2_pagefault_handler(cpu_registers_t* regs);
void vmm2_map_block(uint32_t phys, virtual_addr_t virt, uint32_t flags, page_directory_t* dir);

page_directory_t* vmm2_create_pagedir();

vmm2_status_t vmm2_initialize(uint32_t stack, uint32_t size, uint32_t regionMapLocation)
{
    uint32_t memmapSize = pmm_initialize(stack, size, regionMapLocation);
    //Deinit the physical memory location for the kernel!
    pmm_deinit_region(KERNEL_PHYSICAL, (stack-KERNEL_VIRTUAL) + memmapSize);

    //VMM Init!
    // - Set up kernel directory and use it as current
    _kernel_dir = vmm2_create_pagedir();

    // - Identitymap 0->0x100000
    vmm2_identitymap_todir(0x00000000, 256, VMM2_PAGE_WRITABLE, _kernel_dir); //1Mb (256 blocks)

    // - Map kernel physical to kernel virtual (4Mb)
    // NOTE: All is writable, even .DATA and .BSS
    //       Next bootloader should send us info about these sections!
    vmm2_physicalmap_todir(KERNEL_PHYSICAL, KERNEL_VIRTUAL, 1024, VMM2_PAGE_WRITABLE, _kernel_dir);

    // - Register page fault handler
    irq_register_handler(14, vmm2_pagefault_handler);

    // - Swap out the page directory from bootlader!
    if(vmm2_switch_pagedir(_kernel_dir))
    {
        if(!paging_is_enabled())
        {
            paging_enable(1);
        }
        return VMM2_OK;
    }
    else
    {
        return VMM2_ERROR;
    }
}


void vmm2_identitymap(uint32_t phys, uint32_t blocks, uint32_t flags)
{
    vmm2_physicalmap(phys, phys, blocks, flags);
}

void vmm2_identitymap_todir(uint32_t phys, uint32_t blocks, uint32_t flags, page_directory_t* dir)
{
    vmm2_physicalmap_todir(phys, phys, blocks, flags, dir);
}

void vmm2_physicalmap(uint32_t phys, virtual_addr_t virt, uint32_t blocks, uint32_t flags)
{
    vmm2_physicalmap_todir(phys, virt, blocks, flags, paging_get_CR3());
}

void vmm2_physicalmap_todir(uint32_t phys, virtual_addr_t virt, uint32_t blocks, uint32_t flags, page_directory_t* dir)
{
    unsigned int endAddr = phys + (VMM2_BLOCK_SIZE * blocks);
    //TODO: Check if virt addr is mapped before;
    for (uint32_t phys_addr=phys, virt_addr=virt;
        phys_addr < endAddr;
        phys_addr+=VMM2_BLOCK_SIZE, virt_addr+=VMM2_BLOCK_SIZE)
    {
        pmm_deinit_block(phys_addr);
        vmm2_map_block(phys_addr, virt_addr, flags, dir);
    }
}

void vmm2_map(virtual_addr_t virt, uint32_t blocks, uint32_t flags)
{
    vmm2_map_todir(virt, blocks, flags, paging_get_CR3());
}

void vmm2_map_todir(virtual_addr_t virt, uint32_t blocks, uint32_t flags, page_directory_t* dir)
{
    unsigned int endAddr = virt + (VMM2_BLOCK_SIZE * blocks);
    //TODO: Check if virt addr is mapped before;
    for (uint32_t addr=virt; addr < endAddr; addr+=VMM2_BLOCK_SIZE)
    {
        uint32_t phys_addr = (uint32_t)pmm_alloc_block();
        // printk("Mapping %x to phys %x\n", addr, phys_addr);
        vmm2_map_block(phys_addr, addr, flags, dir);
    }
}

void vmm2_map_block(uint32_t phys, virtual_addr_t virt, uint32_t flags, page_directory_t* dir)
{
    uint32_t* page_table = vmm2_get_pagetable(virt, dir, 1);
    // printk("Mapping: %x to %x (%x)\n", phys, virt, page_table);

    //Set flags.
    *page_table |= flags;

    //Get the physical address to the page table from the 20 bits.
    page_table_t* table = (page_table_t*)ENTRY_PHYS_ADDRESS(page_table);

    //printk("Directory Index: %d - Table Index: %d\n",DIRECTORY_INDEX(virt),TABLE_INDEX(virt));
    uint32_t* page = &table->entries[TABLE_INDEX(virt)];
    *page = (phys | VMM2_PAGE_PRESENT);
    *page |= flags;
#ifdef _USER_DEBUG
    *page |= VMM2_PAGE_USER;
#endif

}

uint32_t* vmm2_get_pagetable(virtual_addr_t virt, page_directory_t* dir, uint32_t create)
{
    uint32_t* page_table = &dir->tables[DIRECTORY_INDEX(virt)];
    // If the page table is 0 then it is unnused,
    // And a new page table has to be created and
    // linked into the page directory entry.
    if(!*page_table && create)
    {
        page_table_t* table = (page_table_t*)pmm_alloc_block();
        memset(table, 0, sizeof(page_table_t));
        *page_table = (uint32_t)table;
        *page_table |= VMM2_PAGE_PRESENT;
#ifdef _USER_DEBUG
        *page_table |= VMM2_PAGE_USER;
#endif
        // printk("Created new page table (%x) for index: %d virt: %x\n", page_table, DIRECTORY_INDEX(virt), virt);
    }
    return page_table;
}

page_directory_t* vmm2_create_pagedir()
{
    page_directory_t* tmp = (page_directory_t*)pmm_alloc_block();
    memset(tmp, 0, sizeof(page_directory_t));
    // printk("Creating new pagedir at %x physical\n", tmp);
    return tmp;
}

vmm2_status_t vmm2_switch_pagedir(page_directory_t* dir)
{
    if (!dir)
    {
        return VMM2_ERROR;
    }
    else
    {
        paging_set_CR3((uint32_t)&dir->tables);
        return VMM2_OK;
    }
}

page_directory_t* vmm2_get_directory()
{
    return paging_get_CR3();
}

page_directory_t* vmm2_create_directory()
{
    page_directory_t* dir = vmm2_create_pagedir();

    //NOTE: When linking index 1 (first 4mb) we are linking the identitymap
    //      used by the kernel. We only need and have1mb identity mapped. The
    //      other 3mb is "wasted". Consider "re-identity-mapping" the 1mb instead?

    for(int i = 0; i<1024; i++)
    {
        if(_kernel_dir->tables[i] != 0)
        {
            dir->tables[i] = _kernel_dir->tables[i];
        }
    }

    return dir;
}

void vmm2_dispose_directory(page_directory_t*)
{
    //TODO!! We are leaking now!
}

void vmm2_pagefault_handler(cpu_registers_t* regs)
{
    uint32_t faulting_address;
	__asm("mov %%cr2, %0" : "=r"(faulting_address));

    int not_present = !(regs->err_code & 0x1);
    int write_operation = regs->err_code & 0x2;
    int user_mode = regs->err_code & 0x4;
    int cpu_reserved = regs->err_code & 0x8;

    printk("Page fault! (");
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
    printk("------------------------------------------------\n");
    page_directory_t* pagedir = paging_get_CR3();
    page_entry_t* page_table = (page_entry_t*)&pagedir->tables[DIRECTORY_INDEX(faulting_address)];
    printk("TYPE       | INDEX | PRESENT | WRITABLE | USER\n");
    printk("Directory  | %d     | %s     | %s       | %s\n",
        DIRECTORY_INDEX(faulting_address),
        page_table->present ? "Yes" : "No",
        page_table->writable ? "Yes" : "No",
        page_table->user ? "Yes" : "No"
    );
    if(page_table->present)
    {
        page_table_t* table = (page_table_t*)ENTRY_PHYS_ADDRESS((uint32_t*)page_table);
        page_entry_t* page = (page_entry_t*)&table->entries[TABLE_INDEX(faulting_address)];
        printk("Index      | %d     | %s     | %s       | %s\n",
            TABLE_INDEX(faulting_address),
            page->present ? "Yes" : "No",
            page->writable ? "Yes" : "No",
            page->user ? "Yes" : "No"
        );
        //print!
    }
    printk("\n");
    kernel_panic("PAGEFAULT", regs);
}
