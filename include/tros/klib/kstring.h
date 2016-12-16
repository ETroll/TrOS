#ifndef _K_STRING_H
#define _K_STRING_H

char* strcpy(char *s1, const char *s2);
char* strncpy(char *s1, const char *s2, unsigned int len);

char* strdup(const char *s1);

unsigned int strlen(const char* str);
char* trimend(char* str);
int strcmp(const char* a, const char* b);
int stricmp (const char *p1, const char *p2);

void* memcpy(void *dest, const void *src, unsigned int count);
void* memset(void *dest, char val, unsigned int count);

int isupper(char c); //static inline ?
int isalpha(char c);
int isspace(char c);
int isdigit(char c);
char toupper(char c);

#endif
