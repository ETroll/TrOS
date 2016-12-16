#ifndef _STRING_H
#define _STRING_H

char* strcpy(char *s1, const char *s2);
char* strncpy(char *s1, const char *s2, unsigned int len);
unsigned int strlen(const char* str);
int strcmp(const char* a, const char* b);

void* memcpy(void *dest, const void *src, unsigned int count);
void* memset(void *dest, char val, unsigned int count);


#endif
