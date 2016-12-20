#ifndef INCLUDE_TROS_DEVICEBUFFER_H
#define INCLUDE_TROS_DEVICEBUFFER_H

#include <tros/process.h>
#include <tros/klib/ringbuffer.h>

typedef struct {
    ringbuffer_t* ringbuffer;
    process_t* listener;
} devicebuffer_t;

devicebuffer_t* devicebuffer_create();
void devicebuffer_free(devicebuffer_t* db);

int devicebuffer_read(devicebuffer_t* db, int* buffer, uint32_t count);
void devicebuffer_write(devicebuffer_t* db, int data);

#endif
