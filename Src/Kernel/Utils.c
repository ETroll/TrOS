#include <Tros/Utils.h>

/*
 * Methods are named with _k_ so the compiler does not cmplain when I create my own libc
 * methods for the "userland" apps. Since the "userland" apps are linked into the same
 * kernel image as the kernel for now, due to lack of file system support, I found it best to do this.
 */
void* _k_memcpy(void *pDest, const void *pSrc, unsigned int n)
{
    char* pData = pDest;
    const char* s = pSrc;

    for(unsigned int i = 0; i < n; i++)
    {
        pData[i] = s[i];
    }

    return pData;
}

void _k_memset(void* mem, unsigned char d, unsigned int n)
{
    char* data = mem;
    for(unsigned int i = 0; i < n; i++)
    {
        data[i] = d;
    }
}

int _k_strcmp(const char* a, const char* b)
{
    int r = 0;
    while(!r && *a && *b)
    {
        r = (*a++) - (*b++);
    }
    return (*a) - (*b);
}

uint _k_strlen(const char* str)
{
    uint size = 0;
    while(*str++)
    {
        size++;
    }
    return size;
}

char* _k_strcpy(char* dest, const char* src)
{
    uint size = 0;
    while(src[size])
    {
    		dest[size] = src[size];
        size++;
    }
    dest[size] = '\0';
    return dest;
}

void rb_init(struct ringbuffer* rb)
{
    rb->start = rb->end = 0;
}

void rb_push(struct ringbuffer* rb, char data)
{
    rb->data[rb->end++] = data;
    if(rb->end > RB_BUF_SIZE)
    {
        rb->end = 0;
    }
}
void rb_pop(struct ringbuffer* rb, char* data)
{
    (*data) = rb->data[rb->start++];
    if(rb->start > RB_BUF_SIZE)
    {
        rb->start = 0;
    }
}
uint rb_len(struct ringbuffer* rb)
{
    return (rb->end - rb->start) + ((rb->end < rb->start) ? RB_BUF_SIZE : 0);
}
