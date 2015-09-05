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

int strcmp(const char* str1, const char* str2)
{
    while(*str1 == *str2)
    {
        if(*str1 == '\0')
        {
            return 0;
        }
        str1++;
        str2++;
    }
    return (*str1 - *str2);
}

unsigned int strlen (const char* str)
{
    unsigned int len=0;
    while (str[len])
    {
        len++;
    }
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
