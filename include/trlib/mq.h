#ifndef INCLUDE_TRLIB_MQ_H
#define INCLUDE_TRLIB_MQ_H

#include <stdint.h>

typedef enum mq_flags
{
    MQ_NOFLAGS = 0x00,
    MQ_RECIEPT = 0x01,
    MQ_NOWAIT = 0x01
}mq_flags_t;


int mq_send(uint32_t dest, void* data, uint32_t size, mq_flags_t flags);
int mq_recv(void* buffer, uint32_t size, mq_flags_t flags);

#endif
