// ringbuffer.h
// Basic ringbuffer implementation

#ifndef INCLUDE_DS_RINGBUFFER_H
#define INCLUDE_DS_RINGBUFFER_H

#define RB_BUF_SIZE 32

typedef struct
{
    unsigned int start;
    unsigned int end;
    int data[RB_BUF_SIZE];
} ringbuffer_t;


void rb_init(ringbuffer_t* rb);
void rb_push(ringbuffer_t* rb, int data);
void rb_pop(ringbuffer_t* rb, int* data);
unsigned int rb_len(ringbuffer_t* rb);


#endif
