#include <tros/memory.h>
#include <tros/tros.h>
#include <tros/mem/vmm2.h>
#include <tros/mem/pmm.h>

extern void kheap_initialize();

void memory_initialize(uint32_t stack, uint32_t size, uint32_t regionMapLocation)
{
    if(vmm2_initialize(stack, size, regionMapLocation) == VMM2_OK)
    {
        kheap_initialize();
    }
    else
    {
        kernel_panic("Could not initialize memory!", 0);
    }
}

void memory_physical_usage(mem_usage_physical_t* out)
{
    if(out != 0)
    {
        out->total = pmm_get_block_count();
        out->used = pmm_get_use_block_count();
        out->free = pmm_get_free_block_count();
        out->blocksize = pmm_get_block_size();
    }
}
