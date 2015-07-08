#include <string.h>

char* strcpy(char* dest, const char* src)
{
    unsigned int size = 0;
    while(src[size])
    {
        dest[size] = src[size];
        size++;
    }
    dest[size] = '\0';
    return dest;
}

unsigned int strlen (const char* str)
{
    unsigned int len=0;
    while (str[len++]);
    return len;
}

void* memcpy(void *dest, const void *src, unsigned int count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--)
    {
        *dp++ = *sp++;
    }
    return dest;
}

void* memset(void *dest, char val, unsigned int count)
{
    unsigned char *temp = (unsigned char *)dest;
    for(; count != 0; count--)
    {
        temp[count] = val;
    }
    return dest;
}
