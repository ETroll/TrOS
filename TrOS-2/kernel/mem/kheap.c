// kheap.c
// Simple kernel heap implementation
// Disclaimer! Not optimal at all! Uses *too much* memspace, and cpu cycles atm..

#include <tros/tros.h>
#include <tros/kheap.h>
#include <tros/pmm.h>
#include <tros/vmm.h>

#define KERNEL_HEAP_START   0xD0000000
#define KERNEL_HEAP_END     0xDFFFFFFF
#define BLOCK_SIZE          4096
#define BLOCK_ALIGN         BLOCK_SIZE

struct heap_chunk_t
{
    struct heap_chunk_t* next;
    struct heap_chunk_t* prev;
    unsigned int size;
    unsigned int inuse;
};

static struct heap_chunk_t* _kheap_start = 0;
static struct heap_chunk_t* _kheap_free = 0;

static struct heap_chunk_t* kheap_extend(unsigned int amount);
static struct heap_chunk_t* kheap_first_free(unsigned int amount);
static void kheap_add_chunk_heap(struct heap_chunk_t* chunk);
static void kheap_add_chunk_free(struct heap_chunk_t* chunk);
static void kheap_add_overflow_to_free(struct heap_chunk_t* chunk, unsigned int size);
static void kheap_add_ordered_chunk(struct heap_chunk_t** start, struct heap_chunk_t* chunk);

void kheap_initialize()
{
    _kheap_free = 0;
    _kheap_start = 0;
}

void* kmalloc(unsigned int size)
{
    struct heap_chunk_t* chunk = 0;

    if(_kheap_free == 0)
    {
        chunk = kheap_extend(size);
    }
    else
    {
        chunk = kheap_first_free(size);
        if(chunk == 0)
        {
            // Is the address of the last free chunk larger than last chunk on the heaplist?
            //   Yes:    Increase with (wanted size)-(sizeof last free chunk)
            //           Combine the old chunk with the new chunk and add to heaplist
            //   No:     Increase with full wanted size and add to heaplist
            if(_kheap_start != 0 && _kheap_free->prev > _kheap_start->prev)
            {
                struct heap_chunk_t* last = _kheap_free->prev;
                chunk = kheap_extend(size - last->size);

                last->size += sizeof(struct heap_chunk_t) + chunk->size;
                chunk = last;
            }
            else
            {
                chunk = kheap_extend(size);
            }
        }
    }

    kheap_add_chunk_heap(chunk);
    if(chunk->size > size)
    {
        kheap_add_overflow_to_free(chunk, size);
    }
    return (void*)((unsigned int)chunk +sizeof(struct heap_chunk_t));
}

void kfree(void* ptr)
{

}

static struct heap_chunk_t* kheap_extend(unsigned int size)
{
    static unsigned int _kheap_next_virtual_addr = KERNEL_HEAP_START;
    unsigned int chunk_start_addr = _kheap_next_virtual_addr;

    int amount = ((size + sizeof(struct heap_chunk_t)) / BLOCK_SIZE);
    if(((size + sizeof(struct heap_chunk_t)) % BLOCK_SIZE) != 0)
    {
        amount++;
    }

    for(int i = 0; i<amount; i++)
    {
        void* phys = pmm_alloc_block();
        vmm_map_page(phys, (void*)_kheap_next_virtual_addr);
        _kheap_next_virtual_addr += BLOCK_SIZE;
    }

    struct heap_chunk_t* newchunk = (struct heap_chunk_t*) chunk_start_addr;
    newchunk->size = (amount*BLOCK_SIZE) - 16;

    return newchunk;
}

static struct heap_chunk_t* kheap_first_free(unsigned int amount)
{
    if(_kheap_free != 0)
    {
        struct heap_chunk_t* iterator = _kheap_free;
        struct heap_chunk_t* ret = 0;
        do
        {
            if(iterator->size >= amount)
            {
                ret = iterator;
                break;
            }
            iterator = iterator->next;
        } while(iterator->next != _kheap_free);
        return ret;
    }
    else
    {
        return 0;
    }
}

static void kheap_add_overflow_to_free(struct heap_chunk_t* chunk, unsigned int size)
{
    struct heap_chunk_t* overflow = (struct heap_chunk_t*)((unsigned int)chunk
        + sizeof(struct heap_chunk_t)
        + size);
    overflow->size = chunk->size - size - sizeof(struct heap_chunk_t);
    chunk->size = size;
    kheap_add_chunk_free(overflow);
}

static void kheap_add_chunk_free(struct heap_chunk_t* chunk)
{
    if(chunk == _kheap_free)
    {
        printk("Chunk == _kheap_free!\n");
    }

    if(_kheap_free == 0)
    {
        _kheap_free = chunk;
        _kheap_free->next = _kheap_free;
        _kheap_free->prev = _kheap_free;
    }
    else
    {
        struct heap_chunk_t* iterator =  _kheap_free;
        do
        {
            if(iterator < chunk)
            {
                chunk->next = iterator->next;
                chunk->prev = iterator;
                iterator->next = chunk;
                chunk->next->prev = chunk;
                break;
            }
            iterator = iterator->prev;
        } while(iterator->prev != _kheap_free);
    }
}

static void kheap_add_chunk_heap(struct heap_chunk_t* chunk)
{
    if(chunk == _kheap_free)
    {

        if(_kheap_free->next == _kheap_free)
        {
            _kheap_free = 0;
        }
        else
        {
            _kheap_free = _kheap_free->next;
        }
    }

    if(_kheap_start == 0)
    {
        _kheap_start = chunk;
        _kheap_start->next = _kheap_start;
        _kheap_start->prev = _kheap_start;
    }
    else
    {
        struct heap_chunk_t* iterator =  _kheap_start;
        do
        {
            if(iterator < chunk)
            {
                chunk->next = iterator->next;
                chunk->prev = iterator;
                iterator->next = chunk;
                chunk->next->prev = chunk;
                break;
            }
            iterator = iterator->prev;
        } while(iterator->prev != _kheap_start);
    }
}
