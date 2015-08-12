// kheap.h
// Kernel Heap

#ifndef INCLUDE_TROS_KHEAP_H
#define INCLUDE_TROS_KHEAP_H

void kheap_initialize();

void* kmalloc(unsigned int size);

#endif
