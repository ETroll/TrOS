#ifndef _STRING_H
#define _STRING_H

#define RB_BUF_SIZE 512

#define TRUE    1
#define FALSE   0
#define NULL    0

#define SIZE_WORD  4
#define SIZE_HWORD 2

typedef unsigned int uint;
typedef unsigned char byte;

struct ringbuffer
{
    uint start;
    uint end;
    char data[RB_BUF_SIZE];
};

//---- Memory
void* _k_memcpy(void *dest, const void *src, unsigned int n);
void _k_memset(void* mem, unsigned char d, unsigned int n);

//---- String
uint _k_strlen(const char* str);
int _k_strcmp(const char* a, const char* b);
char* _k_strcpy(char* dest, const char* src);
//void sprintf(void (*functionPtr)(unsigned int), char* str, ...);

//---- Ringbuffer
void rb_init(struct ringbuffer* rb);
void rb_push(struct ringbuffer* rb, char data);
void rb_pop(struct ringbuffer* rb, char* data);
uint rb_len(struct ringbuffer* rb);

#endif
