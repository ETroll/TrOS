#ifndef SRC_INCLUDE_STRING_H_
#define SRC_INCLUDE_STRING_H_

//#define NULL	0

void* memcpy(void *pDest, const void *pSrc, unsigned int n);

void memset(void* mem, unsigned char d, unsigned int n);

int strcmp(const char* a, const char* b);

unsigned int strlen(const char* str);

char* strcpy(char* dest, const char* src);

#endif /* SRC_INCLUDE_STRING_H_ */
