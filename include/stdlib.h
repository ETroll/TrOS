#ifndef INCLUDE_STDLIB_H
#define INCLUDE_STDLIB_H

#include <stdint.h>

#ifndef NULL
#define NULL	0
#endif

void* malloc(uint32_t size);
void free(void* ptr);


#endif
