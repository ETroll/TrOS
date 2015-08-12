#include <tros/mmap.h>
#include <string.h>
#include <tros/tros.h>

static unsigned int* __mmap_memory_map = 0;
static unsigned int __mmap_max_blocks = 0;

void mmap_initialize(unsigned int phys_loc, unsigned int max_blocks)
{
    __mmap_memory_map = (unsigned int*)phys_loc;
    __mmap_max_blocks = max_blocks;

    for(int i = 0; i<(max_blocks / 32); i++)
    {
        __mmap_memory_map[i] = 0xFFFFFFFF;
    }

    //memset (__mmap_memory_map, 0xf, (__mmap_max_blocks / bpb));
}

void mmap_set_used(int block)
{
    __mmap_memory_map[block / 32] |= (1 << (block % 32));
}

void mmap_set_notused(int block)
{
    __mmap_memory_map[block / 32] &= ~ (1 << (block % 32));
}

int mmap_test_block(int block)
{
    return __mmap_memory_map[block / 32] &  (1 << (block % 32));
}

int mmap_get_first_free_block()
{
    for(unsigned int i=0; i < (__mmap_max_blocks / 32); i++)
    {
        if(__mmap_memory_map[i] != 0xffffffff)
        {
            //We have at least one free frame, lets test the bits and find which one.
            for(int j=0; j<32; j++)
            {
                int bit = 1 << j;
                if(!(__mmap_memory_map[i] & bit))
                {
                    return i*4*8+j;
                }
            }
        }
    }
    return -1;
}

int mmap_get_first_free_size(unsigned int size)
{
    //printk("Searching for %d free blocks %x\n", size, __mmap_memory_map);
    if(size > 0)
    {
        if(size==1)
        {
            return mmap_get_first_free_block();
        }
        else
        {
            for(unsigned int i=0; i < (__mmap_max_blocks / 32); i++)
            {
                if(__mmap_memory_map[i] != 0xffffffff)
                {
                    for(int j=0; j<32; j++)
                    {
                        int bit = 1 << j;
                        if(!(__mmap_memory_map[i] & bit))
                        {

                            int start_bit = (i*32) + j;

                            //printk("Found bit %d : start_bit: %d\n", j, start_bit);

                            unsigned int free_bits = 0;
                            for(unsigned int count=0; count<=size; count++)
                            {
                                //printk("Testing %d\n", start_bit+count);
                                if(!mmap_test_block(start_bit+count))
                                {
                                    free_bits++;
                                    //printk("One sequencial block\n");
                                }
                                if(free_bits == size)
                                {
                                    return i*4*8+j;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return -1;

}
