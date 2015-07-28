#include <ds/ringbuffer.h>

void rb_init(ringbuffer_t* rb)
{
    rb->start = rb->end = 0;
}

void rb_push(ringbuffer_t* rb, int data)
{
    rb->data[rb->end++] = data;
    if(rb->end > RB_BUF_SIZE)
    {
        rb->end = 0;
    }
}

void rb_pop(ringbuffer_t* rb, int* data)
{
    (*data) = rb->data[rb->start++];
    if(rb->start > RB_BUF_SIZE)
    {
        rb->start = 0;
    }
}

unsigned int rb_len(ringbuffer_t* rb)
{
    return (rb->end - rb->start) + ((rb->end < rb->start) ? RB_BUF_SIZE : 0);
}
