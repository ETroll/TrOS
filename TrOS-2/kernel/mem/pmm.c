#include <tros/mem/mmap.h>
#include <tros/mem/pmm.h>
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

typedef struct {
    unsigned int startLo;
    unsigned int startHi;
    unsigned int sizeLo;
    unsigned int sizeHi;
    unsigned int type;
    unsigned int acpi_3_0;
} pmm_region_t;

int pmm_initialize(physical_addr_t bitmap, unsigned int size, physical_addr_t regionMapLocation)
{
    __pmm_memory_size = size;
    __pmm_max_blocks =	((__pmm_memory_size*1024) / PMM_BLOCK_SIZE);
    __pmm_used_blocks = __pmm_max_blocks;
    pmm_region_t* regions = (pmm_region_t*)regionMapLocation;

    mmap_initialize(bitmap, __pmm_max_blocks);

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
            regions[i].sizeLo / 1024,
            regions[i].type,
            pmm_memory_types[regions[i].type-1]);

        if (regions[i].type == 1) // && regions[i].startLo != 0)
        {
            pmm_init_region(regions[i].startLo, regions[i].sizeLo);
        }
    }
    mmap_set_used(0); //0x00000000 used for "out of mem"
    mmap_set_used(1);

	return sizeof(unsigned int) * (__pmm_max_blocks / 32);
}

void pmm_init_region(physical_addr_t addr, unsigned int size)
{
    //TODO: Ignore if requestet region is 0?

    unsigned int start_block = addr / PMM_BLOCK_SIZE;
    unsigned int num_blocks = size / PMM_BLOCK_SIZE;
    unsigned int end_block = start_block + num_blocks;

	//printk("Init region for use. Start %d, stop %d\n", start_block, end_block);
    for (int i = start_block; i<=end_block; i++)
    {
        mmap_set_notused(i);
        __pmm_used_blocks--;
    }
}

void pmm_deinit_region(physical_addr_t addr, unsigned int size)
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

void pmm_deinit_block(physical_addr_t addr)
{
    if(addr % PMM_BLOCK_SIZE == 0)
    {
        if(!mmap_test_block(addr))
        {
            mmap_set_used(addr);
            __pmm_used_blocks++;
        }
    }
    else
    {
        printk("ERROR! Physical block marked as used is not 4K aligned\n");
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

	//printk("Allocating block %d (%x)\n", block, addr);

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

	//printk("Allocating %d blocks %d (%x)\n", size, start_block, block_addr);

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
