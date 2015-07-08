#ifndef _STRING_H
#define _STRING_H

char* strcpy(char *s1, const char *s2);
unsigned int strlen(const char* str );

void* memcpy(void *dest, const void *src, unsigned int count);
void* memset(void *dest, char val, unsigned int count);

#endif
