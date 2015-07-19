// mmap.h
// Memory bitmap functions. 1 bit represents a block.

#ifndef INCLUDE_TROS_MMAP_H
#define INCLUDE_TROS_MMAP_H

void mmap_initialize(unsigned int phys_loc, unsigned int max_blocks, unsigned int bpb);
void mmap_set_used(int block);
void mmap_set_notused(int block);
int mmap_test_block(int block);
int mmap_get_first_free_block();
int mmap_get_first_free_size(unsigned int size);

#endif
