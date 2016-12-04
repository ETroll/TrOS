// kheap.c
// Simple kernel heap implementation
// Disclaimer! Not optimal at all! Uses *too much* memspace, and cpu cycles atm..

#include <tros/tros.h>
// #include <tros/memory.h>
#include <tros/mem/vmm2.h>

#define KERNEL_HEAP_START   0xD0000000
#define KERNEL_HEAP_END     0xDFFFFFFF
#define CHUNK_ID            0xCAFEB00D
#define BLOCK_SIZE          4096
#define BLOCK_ALIGN         BLOCK_SIZE

struct heap_chunk_t
{
    struct heap_chunk_t* next;
    struct heap_chunk_t* prev;
    unsigned int size;
    unsigned int chend;
};

static struct heap_chunk_t* _kheap_start = 0;
static struct heap_chunk_t* _kheap_free = 0;

static struct heap_chunk_t* kheap_extend(unsigned int amount);
static struct heap_chunk_t* kheap_first_free(unsigned int amount);
static void kheap_add_chunk_heap(struct heap_chunk_t* chunk);
static void kheap_remove_chunk_heap(struct heap_chunk_t* chunk);
static void kheap_add_chunk_free(struct heap_chunk_t* chunk);
static void kheap_remove_chunk_free(struct heap_chunk_t* chunk);
static void kheap_add_overflow_to_free(struct heap_chunk_t* chunk, unsigned int size);
static void kheap_merge_freelist_bottom(struct heap_chunk_t* chunk);
static void kheap_merge_freelist_top(struct heap_chunk_t* chunk);

//debug function
// static void kheap_debug_printlist(char* title, struct heap_chunk_t* listhead);
static int kheap_validate(struct heap_chunk_t* listhead);

void kheap_initialize()
{
    _kheap_free = 0;
    _kheap_start = 0;
}

void* kmalloc(unsigned int size)
{
    // printk("--kmalloc- \n");
    // printk("Allocating: %d bytes \n", size);
    // kheap_debug_printlist("Freelist Before:", _kheap_free);
    if(!kheap_validate(_kheap_start))
    {
        printk("ERROR: heaplist corrupted!\n");
        return 0;
    }
    if(!kheap_validate(_kheap_free))
    {
        printk("ERROR: freeist corrupted!\n");
        return 0;
    }

    struct heap_chunk_t* chunk = 0;

    if(_kheap_free == 0)
    {
        //printk("E ");
        chunk = kheap_extend(size);
        //printk("%x ", chunk);
    }
    else
    {
        // printk("S ");
        chunk = kheap_first_free(size);
        if(chunk == 0)
        {
            // printk("F0 %x ",_kheap_free);
            // Is the address of the last free chunk larger than last chunk on the heaplist?
            //   Yes:    Increase with (wanted size)-(sizeof last free chunk)
            //           Combine the old chunk with the new chunk and add to heaplist
            //   No:     Increase with full wanted size and add to heaplist
            if(_kheap_start != 0 && _kheap_free->prev > _kheap_start->prev)
            {
                struct heap_chunk_t* last = _kheap_free->prev;
                //TODO: if((int)(size - last->size) > 0)

                kheap_remove_chunk_free(last);
                chunk = kheap_extend(size - last->size);
                last->size += sizeof(struct heap_chunk_t) + chunk->size;
                chunk = last;
            }
            else
            {
                chunk = kheap_extend(size);
            }
        }
        else
        {
            // printk("C %x ", chunk);
            kheap_remove_chunk_free(chunk);
        }
    }
    // printk("A ");
    kheap_add_chunk_heap(chunk);
    if(chunk->size > size)
    {
        // printk("OV ");
        kheap_add_overflow_to_free(chunk, size);
    }
    //kheap_debug_printlist("Freelist after:", _kheap_free);
    // printk("--/kmalloc- Returned %x \n", chunk);
    return (void*)((unsigned int)chunk +sizeof(struct heap_chunk_t));
}

void kfree(void* ptr)
{
    // printk("--kfree- \n");
    if(!kheap_validate(_kheap_start))
    {
        printk("ERROR: heaplist corrupted!\n");
        return;
    }
    if(!kheap_validate(_kheap_free))
    {
        printk("ERROR: freeist corrupted!\n");
        return;
    }
    struct heap_chunk_t* chunk = (struct heap_chunk_t*) (ptr-sizeof(struct heap_chunk_t));
    // printk("Chunk: %x size: %d\n", chunk, chunk->size);
    // kheap_debug_printlist("Freelist before:", _kheap_free);
    if(chunk->chend == CHUNK_ID)
    {
        kheap_remove_chunk_heap(chunk);
        kheap_add_chunk_free(chunk);
    }
    else
    {
        //PANIC!
        printk("ERROR! - Trying to free unallocated memory at %x\n", ptr);
    }
    // kheap_debug_printlist("Freelist after:", _kheap_free);
    // printk("--/kfree- \n");
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

    vmm2_map(_kheap_next_virtual_addr, amount, VMM2_PAGE_WRITABLE);
    _kheap_next_virtual_addr += (amount * BLOCK_SIZE);

    struct heap_chunk_t* newchunk = (struct heap_chunk_t*) chunk_start_addr;
    newchunk->size = (amount*BLOCK_SIZE) - 16; //TODO: sizeof(struct heap_chunk_t) instead of 16
    newchunk->chend = CHUNK_ID;
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
        } while(iterator != _kheap_free);
        return ret;
    }
    else
    {
        return 0;
    }
}

static void kheap_add_overflow_to_free(struct heap_chunk_t* chunk, unsigned int size)
{
    if((int)(chunk->size - size - sizeof(struct heap_chunk_t)) > 0)
    {
        struct heap_chunk_t* overflow = (struct heap_chunk_t*)((unsigned int)chunk
            + sizeof(struct heap_chunk_t)
            + size);
        overflow->size = chunk->size - size - sizeof(struct heap_chunk_t);
        overflow->chend = CHUNK_ID;
        chunk->size = size;
        kheap_add_chunk_free(overflow);
    }
}

static void kheap_add_chunk_free(struct heap_chunk_t* chunk)
{
    if(_kheap_free == 0)
    {
        // printk("F == 0 ");
        _kheap_free = chunk;
        _kheap_free->next = _kheap_free;
        _kheap_free->prev = _kheap_free;
    }
    else if(chunk < _kheap_free)
    {
        // printk("C < F ");
        chunk->next = _kheap_free;
        chunk->prev = _kheap_free->prev;
        _kheap_free->prev->next = chunk;
        _kheap_free->prev = chunk;
        _kheap_free = chunk;

    }
    else
    {
        // printk("C %x ", chunk);
        // kheap_debug_printlist("Freelist ", _kheap_free);
        unsigned char found = 0;
        struct heap_chunk_t* iterator =  _kheap_free->prev;
        do
        {
            // printk("%x->",iterator);
            if(iterator < chunk)
            {
                // printk("(%x < %x) ", iterator, chunk);
                chunk->next = iterator->next;
                chunk->prev = iterator;
                iterator->next->prev = chunk;
                iterator->next = chunk;
                found = 1;
                break;
            }
            iterator = iterator->prev;
        } while(iterator != _kheap_free->prev);

        if(!found)
        {
            printk("ERROR: Chunk not added to freelist");
        }
    }
    kheap_merge_freelist_bottom(chunk);
    kheap_merge_freelist_top(chunk);
}

static void kheap_add_chunk_heap(struct heap_chunk_t* chunk)
{
    //TODO: Check, do I still need this? (Since it should be taken care of before)
    // if(chunk == _kheap_free)
    // {
    //     if(_kheap_free->next == _kheap_free)
    //     {
    //         _kheap_free = 0;
    //     }
    //     else
    //     {
    //         _kheap_free->prev->next = _kheap_free->next;
    //         _kheap_free->next->prev = _kheap_free->prev;
    //         _kheap_free = _kheap_free->next;
    //     }
    // }
    if(_kheap_start == 0)
    {
        _kheap_start = chunk;
        _kheap_start->next = _kheap_start;
        _kheap_start->prev = _kheap_start;
    }
    else if(chunk < _kheap_start)
    {
        chunk->next = _kheap_start;
        chunk->prev = _kheap_start->prev;
        _kheap_start->prev->next = chunk;
        _kheap_start->prev = chunk;
        _kheap_start = chunk;
    }
    else
    {
        struct heap_chunk_t* iterator =  _kheap_start->prev;
        do
        {
            if(iterator < chunk)
            {
                chunk->next = iterator->next;
                chunk->prev = iterator;
                iterator->next->prev = chunk;
                iterator->next = chunk;

                break;
            }
            iterator = iterator->prev;
        } while(iterator != _kheap_start->prev);
    }
}

static void kheap_remove_chunk(struct heap_chunk_t* chunk)
{
    chunk->prev->next = chunk->next;
    chunk->next->prev = chunk->prev;
    chunk->next = 0;
    chunk->prev = 0;
}

static void kheap_remove_chunk_heap(struct heap_chunk_t* chunk)
{
    if(chunk == _kheap_start)
    {
        if(_kheap_start->next == _kheap_start)
        {
            _kheap_start = 0;
        }
        else
        {
            _kheap_start = _kheap_start->next;
        }
    }
    kheap_remove_chunk(chunk);
}

static void kheap_remove_chunk_free(struct heap_chunk_t* chunk)
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
    kheap_remove_chunk(chunk);
}

static void kheap_merge_freelist_bottom(struct heap_chunk_t* chunk)
{
    struct heap_chunk_t* next = chunk->next;

    unsigned int chunk_end = (unsigned int)chunk
        + sizeof(struct heap_chunk_t)
        + chunk->size;

    if((struct heap_chunk_t*)chunk_end == chunk->next)
    {
        chunk->size += next->size + sizeof(struct heap_chunk_t);
        chunk->next = next->next;
        next->next->prev = chunk;
    }
}

static void kheap_merge_freelist_top(struct heap_chunk_t* chunk)
{
    struct heap_chunk_t* prev = chunk->prev;

    unsigned int prev_end = (unsigned int)prev
        + sizeof(struct heap_chunk_t)
        + prev->size;

    if((unsigned int)chunk == prev_end)
    {
        prev->size += chunk->size + sizeof(struct heap_chunk_t);
        prev->next = chunk->next;
        prev->next->prev = prev;
    }
}

// static void kheap_debug_printlist(char* title, struct heap_chunk_t* listhead)
// {
//     printk("%s: ", title);
//     if(listhead != 0)
//     {
//         struct heap_chunk_t* iterator =  _kheap_free;
//         do
//         {
//             printk("%x (%d)-",iterator, iterator->size);
//             if(iterator->chend != CHUNK_ID) printk("ERROR! ");
//             iterator = iterator->next;
//         } while(iterator != _kheap_free);
//     }
//     else
//     {
//         printk("empty");
//     }
//     printk("\n");
// }

static int kheap_validate(struct heap_chunk_t* listhead)
{
    int success = 1;
    if(listhead != 0)
    {
        struct heap_chunk_t* iterator =  _kheap_free;
        do
        {
            if(iterator->chend != CHUNK_ID)
            {
                success = 0;
            }
            iterator = iterator->next;
        } while(iterator != _kheap_free);
    }
    return success;
}
