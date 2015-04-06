#include <string.h>

void* memcpy(void *pDest, const void *pSrc, unsigned int n)
{
    char* pData = pDest;
    const char* s = pSrc;

    for(unsigned int i = 0; i < n; i++)
    {
        pData[i] = s[i];
    }

    return pData;
}

void memset(void* mem, unsigned char d, unsigned int n)
{
    char* data = mem;
    for(unsigned int i = 0; i < n; i++)
    {
        data[i] = d;
    }
}

int strcmp(const char* s1, const char* s2)
{
	while(*s1==*s2)
	{
		if (*s1 == '\0' || *s1 == '\0')
		{
			break;
		}
		s1++;
		s2++;
	}
	if (*s1 == '\0' && *s1 == '\0')
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

unsigned int strlen(const char* str)
{
    unsigned int size = 0;
    while(*str++)
    {
        size++;
    }
    return size;
}

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
