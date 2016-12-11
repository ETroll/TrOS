
#include <syscall.h>

#define BLOCK_SIZE  4096
#define CHUNK_ID    0xDADABAAD

struct heap_chunk_t
{
    struct heap_chunk_t* next;
    struct heap_chunk_t* prev;
    unsigned int size;
    unsigned int chend;
};

static struct heap_chunk_t* _heap_start = 0;
static struct heap_chunk_t* _heap_free = 0;

static struct heap_chunk_t* heap_extend(unsigned int amount);
static struct heap_chunk_t* heap_first_free(unsigned int amount);
static void heap_add_chunk_heap(struct heap_chunk_t* chunk);
static void heap_remove_chunk_heap(struct heap_chunk_t* chunk);
static void heap_add_chunk_free(struct heap_chunk_t* chunk);
static void heap_remove_chunk_free(struct heap_chunk_t* chunk);
static void heap_add_overflow_to_free(struct heap_chunk_t* chunk, unsigned int size);
static void heap_merge_freelist_bottom(struct heap_chunk_t* chunk);
static void heap_merge_freelist_top(struct heap_chunk_t* chunk);

static int heap_validate(struct heap_chunk_t* listhead);

void* malloc(unsigned int size)
{
    if(!heap_validate(_heap_start))
    {
        return 0;
    }
    if(!heap_validate(_heap_free))
    {
        return 0;
    }

    struct heap_chunk_t* chunk = 0;

    if(_heap_free == 0)
    {
        chunk = heap_extend(size);
    }
    else
    {
        chunk = heap_first_free(size);
        if(chunk == 0)
        {
            if(_heap_start != 0 && _heap_free->prev > _heap_start->prev)
            {
                struct heap_chunk_t* last = _heap_free->prev;
                heap_remove_chunk_free(last);
                chunk = heap_extend(size - last->size);
                last->size += sizeof(struct heap_chunk_t) + chunk->size;
                chunk = last;
            }
            else
            {
                chunk = heap_extend(size);
            }
        }
        else
        {
            heap_remove_chunk_free(chunk);
        }
    }
    heap_add_chunk_heap(chunk);
    if(chunk->size > size)
    {
        heap_add_overflow_to_free(chunk, size);
    }
    return (void*)((unsigned int)chunk +sizeof(struct heap_chunk_t));
}

void free(void* ptr)
{
    if(!heap_validate(_heap_start))
    {
        return;
    }
    if(!heap_validate(_heap_free))
    {
        return;
    }
    struct heap_chunk_t* chunk = (struct heap_chunk_t*) (ptr-sizeof(struct heap_chunk_t));

    if(chunk->chend == CHUNK_ID)
    {
        heap_remove_chunk_heap(chunk);
        heap_add_chunk_free(chunk);
    }
    else
    {
        //TODO: exit syscall! Trying to free unallocated memory
    }
}

static struct heap_chunk_t* heap_extend(unsigned int size)
{
    int blocks = ((size + sizeof(struct heap_chunk_t)) / BLOCK_SIZE);
    if(((size + sizeof(struct heap_chunk_t)) % BLOCK_SIZE) != 0)
    {
        blocks++;
    }
    struct heap_chunk_t* newchunk = (struct heap_chunk_t*) syscall_increasemem(blocks);
    newchunk->size = (blocks * BLOCK_SIZE) - sizeof(struct heap_chunk_t);
    newchunk->chend = CHUNK_ID;
    return newchunk;
}

static struct heap_chunk_t* heap_first_free(unsigned int amount)
{
    if(_heap_free != 0)
    {
        struct heap_chunk_t* iterator = _heap_free;
        struct heap_chunk_t* ret = 0;
        do
        {
            if(iterator->size >= amount)
            {
                ret = iterator;
                break;
            }
            iterator = iterator->next;
        } while(iterator != _heap_free);
        return ret;
    }
    else
    {
        return 0;
    }
}

static void heap_add_overflow_to_free(struct heap_chunk_t* chunk, unsigned int size)
{
    if((int)(chunk->size - size - sizeof(struct heap_chunk_t)) > 0)
    {
        struct heap_chunk_t* overflow = (struct heap_chunk_t*)((unsigned int)chunk
            + sizeof(struct heap_chunk_t)
            + size);
        overflow->size = chunk->size - size - sizeof(struct heap_chunk_t);
        overflow->chend = CHUNK_ID;
        chunk->size = size;
        heap_add_chunk_free(overflow);
    }
}

static void heap_add_chunk_free(struct heap_chunk_t* chunk)
{
    if(_heap_free == 0)
    {
        _heap_free = chunk;
        _heap_free->next = _heap_free;
        _heap_free->prev = _heap_free;
    }
    else if(chunk < _heap_free)
    {
        chunk->next = _heap_free;
        chunk->prev = _heap_free->prev;
        _heap_free->prev->next = chunk;
        _heap_free->prev = chunk;
        _heap_free = chunk;

    }
    else
    {
        struct heap_chunk_t* iterator =  _heap_free->prev;
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
        } while(iterator != _heap_free->prev);
    }
    heap_merge_freelist_bottom(chunk);
    heap_merge_freelist_top(chunk);
}

static void heap_add_chunk_heap(struct heap_chunk_t* chunk)
{
    if(_heap_start == 0)
    {
        _heap_start = chunk;
        _heap_start->next = _heap_start;
        _heap_start->prev = _heap_start;
    }
    else if(chunk < _heap_start)
    {
        chunk->next = _heap_start;
        chunk->prev = _heap_start->prev;
        _heap_start->prev->next = chunk;
        _heap_start->prev = chunk;
        _heap_start = chunk;
    }
    else
    {
        struct heap_chunk_t* iterator =  _heap_start->prev;
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
        } while(iterator != _heap_start->prev);
    }
}

static void heap_remove_chunk(struct heap_chunk_t* chunk)
{
    chunk->prev->next = chunk->next;
    chunk->next->prev = chunk->prev;
    chunk->next = 0;
    chunk->prev = 0;
}

static void heap_remove_chunk_heap(struct heap_chunk_t* chunk)
{
    if(chunk == _heap_start)
    {
        if(_heap_start->next == _heap_start)
        {
            _heap_start = 0;
        }
        else
        {
            _heap_start = _heap_start->next;
        }
    }
    heap_remove_chunk(chunk);
}

static void heap_remove_chunk_free(struct heap_chunk_t* chunk)
{
    if(chunk == _heap_free)
    {
        if(_heap_free->next == _heap_free)
        {
            _heap_free = 0;
        }
        else
        {
            _heap_free = _heap_free->next;
        }
    }
    heap_remove_chunk(chunk);
}

static void heap_merge_freelist_bottom(struct heap_chunk_t* chunk)
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

static void heap_merge_freelist_top(struct heap_chunk_t* chunk)
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

static int heap_validate(struct heap_chunk_t* listhead)
{
    int success = 1;
    if(listhead != 0)
    {
        struct heap_chunk_t* iterator =  _heap_free;
        do
        {
            if(iterator->chend != CHUNK_ID)
            {
                success = 0;
            }
            iterator = iterator->next;
        } while(iterator != _heap_free);
    }
    return success;
}
