#include <tros/mmap.h>
#include <tros/pmm.h>

#define PMM_BLOCKS_PER_BYTE 8
#define PMM_BLOCK_SIZE      4096
#define PMM_BLOCK_ALIGN     PMM_BLOCK_SIZE

static unsigned int __pmm_memory_size = 0;
static unsigned int __pmm_used_blocks = 0;
static unsigned int __pmm_max_blocks = 0;


void pmm_initialize(unsigned int addr, unsigned int size)
{
    __pmm_memory_size = size;
    __pmm_max_blocks =	((__pmm_memory_size*1024) / PMM_BLOCK_SIZE);
    __pmm_used_blocks = __pmm_max_blocks;

    mmap_initialize(addr, __pmm_max_blocks, PMM_BLOCKS_PER_BYTE);
}

void pmm_init_region(unsigned int addr, unsigned int size)
{
    //TODO: Ignore if requestet region is 0?

    unsigned int start_block = addr / PMM_BLOCK_SIZE;
    unsigned int num_blocks = size / PMM_BLOCK_SIZE;
    unsigned int end_block = start_block + num_blocks;

    for (int i = start_block; i<=end_block; i++)
    {
        mmap_set_notused(i);
        __pmm_used_blocks--;
    }
    //first block is always set. This insures allocs cant be 0
    mmap_set_used(0);
}

void pmm_deinit_region(unsigned int addr, unsigned int size)
{
    int start_block = addr / PMM_BLOCK_SIZE;
    int num_blocks = size / PMM_BLOCK_SIZE;
    unsigned int end_block = start_block + num_blocks;

    for (int i = start_block; i<=end_block; i++)
    {
        mmap_set_used(i);
        __pmm_used_blocks++;
    }
}

void* pmm_alloc_block()
{
    if(pmm_get_free_block_count() > 0)
    {
        return 0;	//out of memory
    }

    int block = mmap_get_first_free_block();
    if (block == -1)
    {
        return 0;	//out of memory
    }

    mmap_set_used(block);
    unsigned int addr = block * PMM_BLOCK_SIZE;
    __pmm_used_blocks++;

    return (void*)addr;
}

void pmm_free_block(void* blockptr)
{
    unsigned int addr = (unsigned int)blockptr;
	int block = addr / PMM_BLOCK_SIZE;

	mmap_set_notused(block);
	__pmm_used_blocks--;
}

void* pmm_alloc_blocks(unsigned int size)
{
    if(pmm_get_free_block_count() > 0)
    {
        return 0;	//out of memory
    }

    int start_block = mmap_get_first_free_size(size);
    if (start_block == -1)
    {
        return 0;	//out of memory
    }

    for (int i = 0;  i < size; i++)
    {
        mmap_set_used(start_block+i);
    }
    unsigned int block_addr = start_block * PMM_BLOCK_SIZE;
    __pmm_used_blocks+=size;

    return (void*)block_addr;
}

void pmm_free_blocks(void* blockptr, unsigned int size)
{
    unsigned int addr = (unsigned int)blockptr;
	int start_block = addr / PMM_BLOCK_SIZE;

    for (int i = 0;  i < size; i++)
    {
        mmap_set_used(start_block+i);
    }
    __pmm_used_blocks-=size;
}

unsigned int pmm_get_memory_size()
{
    return __pmm_memory_size;
}

unsigned int pmm_get_use_block_count()
{
    return __pmm_used_blocks;
}

unsigned int pmm_get_free_block_count()
{
    return __pmm_max_blocks - __pmm_used_blocks;
}

unsigned int pmm_get_block_count()
{
    return __pmm_max_blocks;
}

unsigned int pmm_get_block_size()
{
    return PMM_BLOCK_SIZE;
}

//TODO: Move these inline tasks in to the HAL instead
void pmm_paging_enable(int enable)
{


}

int pmm_is_paging_enabled()
{
    unsigned int result = 0;
    __asm("mov %0, %%cr0" : "=r" (result));
    return (result & 0x80000000) ? 0 : 1;
}

void pmm_load_PDBR(unsigned int physical_addr)
{
    __asm("mov %%cr3, %0" : : "r" (physical_addr));
}

unsigned int pmmngr_get_PDBR()
{
    unsigned int result = 0;
    __asm("mov %0, %%cr3" : "=r" (result));
    return result;
}
