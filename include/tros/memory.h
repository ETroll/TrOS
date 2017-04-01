/*
    "Public" exposed headerfile for memory operations
    in the TrOS Kernel.
*/

#ifndef INCLUDE_TROS_MEMORY_H
#define INCLUDE_TROS_MEMORY_H

#include <stdint.h>

#define KERNEL_STACK_SIZE 16384

typedef struct
{
    uint32_t total;
    uint32_t used;
    uint32_t free;
    uint32_t blocksize;
} mem_usage_physical_t;

void memory_initialize(uint32_t stack, uint32_t size, uint32_t regionMapLocation);
void memory_physical_usage(mem_usage_physical_t* out);


//Kernel Heap allocation
void* kmalloc(unsigned int size);
void kfree(void* ptr);
#endif
