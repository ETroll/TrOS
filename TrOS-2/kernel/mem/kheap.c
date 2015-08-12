// kheap.c
// Simple kernel heap implementation

#include <tros/tros.h>
#include <tros/kheap.h>
#include <tros/pmm.h>
#include <tros/vmm.h>

#define KERNEL_HEAP_START  0xD0000000
#define KERNEL_HEAP_END    0xDFFFFFFF

typedef struct heap_hunk
{
    struct heap_chunk* next;
    struct heap_chunk* prev;
    unsigned int size;
    unsigned int inuse;
} heap_chunk_t;

static heap_chunk_t* _kheap_start;
static heap_chunk_t* _kheap_free;




void kheap_initialize()
{
    void* initial_block = pmm_alloc_block();
    vmm_map_page(initial_block, (void*)KERNEL_HEAP_START);

    char* data = (char*)0x003EF000;
    // printk("Data at: %x\n", data);
    data[0] = 'c';

    char* otherdata = (char*)0xC02EF000;
    if(data[0] == otherdata[0])
    {
        printk("Proof of concept! 0x003EF000 == 0xC02EF000\n");
    }
}
