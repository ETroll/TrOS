// mmap.h
// Memory bitmap functions. 1 bit represents a block.

#ifndef INCLUDE_TROS_MMAP_H
#define INCLUDE_TROS_MMAP_H

void mmap_initialize(unsigned int phys_loc, unsigned int max_blocks, unsigned int bpb);

//Mark a bit for a frame as used.
void mmap_set_used(int block);

//Mark a bit for a frame as not used and free
void mmap_set_notused(int block);

//Test if bit for frame is set or not
int mmap_test_block(int block);

//Get the location of the first free block
int mmap_get_first_free_block();

//Get location of first block that has <size> blocks free after it
int mmap_get_first_free_size(unsigned int size);

#endif
