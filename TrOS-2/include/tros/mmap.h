#ifndef INCLUDE_TROS_MMAP_H
#define INCLUDE_TROS_MMAP_H

//! set any bit (frame) within the memory map bit array
void mmap_set(int bit);

//! unset any bit (frame) within the memory map bit array
void mmap_unset(int bit);

//! test any bit (frame) within the memory map bit array
int mmap_test(int bit);
int mmap_first_free_frame();
int mmap_first_free_size(unsigned int size);

#endif
