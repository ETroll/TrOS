#include <tros/mmap.h>
#include <tros/pmm.h>
#include <tros/tros.h>

#define PMM_BLOCKS_PER_BYTE 8
#define PMM_BLOCK_SIZE      4096
#define PMM_BLOCK_ALIGN     PMM_BLOCK_SIZE

static unsigned int __pmm_memory_size = 0;
static unsigned int __pmm_used_blocks = 0;
static unsigned int __pmm_max_blocks = 0;

char* pmm_memory_types[] = {
	"Available",
	"Reserved",
	"ACPI Reclaim",
	"ACPI NVS Memory"
};

void pmm_initialize(phy_address bitmap, unsigned int size, pmm_region_t* regions)
{
    __pmm_memory_size = size;
    __pmm_max_blocks =	((__pmm_memory_size*1024) / PMM_BLOCK_SIZE);
    __pmm_used_blocks = __pmm_max_blocks;

    mmap_initialize(bitmap, __pmm_max_blocks, PMM_BLOCKS_PER_BYTE);

    printk("\nPMM initialized with %d KB physical memory\n", size);

    for(int i=0; i<15; ++i)
    {
        if (regions[i].type > 4)
        {
            regions[i].type = 1;
        }
        if(i > 0 && regions[i].startLo == 0)
        {
            break;
        }
        printk("Region %d: Start %x Length %d KB Type: %d (%s)\n",
            i,
            regions[i].startLo,
            regions[i].sizeLo / 1014,
            regions[i].type,
            pmm_memory_types[regions[i].type-1]);

        if (regions[i].type == 1 && regions[i].startLo != 0)
        {
            pmm_init_region(regions[i].startLo, regions[i].sizeLo);
        }
    }


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
    //First block is always set.
    //This insures allocs cant be 0, since 0 is used for OOM
    //mmap_set_used(0);
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
    if(pmm_get_free_block_count() <= 0)
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
    if(pmm_get_free_block_count() <= 0)
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
