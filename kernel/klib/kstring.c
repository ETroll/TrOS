#include <tros/klib/kstring.h>
#include <tros/memory.h>

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

char* strncpy(char* dest, const char* src, unsigned int len)
{
    unsigned int size = 0;
    while(size < len)
    {
        dest[size] = src[size];
        size++;
    }
    return dest;
}

char* strdup(const char *s1)
{
    unsigned int size = strlen(s1);
    return memcpy(kmalloc(size+1), s1, size+1);
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

int stricmp (const char *p1, const char *p2)
{
    unsigned char *s1 = (unsigned char *)p1;
    unsigned char *s2 = (unsigned char *)p2;
    unsigned char c1, c2;

    do
    {
        c1 = (unsigned char)toupper((int)*s1++);
        c2 = (unsigned char)toupper((int)*s2++);
        if (c1 == '\0')
        {
            return c1 - c2;
        }
    } while (c1 == c2);
    return c1 - c2;
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

char* trimend(char* str)
{
    if(str != 0)
    {
        unsigned int length = strlen(str);
        char* strend = str+length;
        while(isspace(*(--strend)))
        {
            *strend = '\0';
            if(strend == str)
            {
                break;
            }
        }
    }
    return str;
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
    for(int i = 0; i<count; i++)
    {
        temp[i] = val;
    }
    return dest;
}

int isupper(char c) //static inline ?
{
    return (c >= 'A' && c <= 'Z');
}

int isalpha(char c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

int isspace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r');
}

int isdigit(char c)
{
    return (c >= '0' && c <= '9');
}

char toupper(char c)
{
    if( c>='a' && c<='z')
    {
        c += ('A' - 'a');
    }
    return c;
}
