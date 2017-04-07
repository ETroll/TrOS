#ifndef INCLUDE_TRLIB_DEVICE_H
#define INCLUDE_TRLIB_DEVICE_H

#include <stdint.h>

typedef unsigned int device_t;

device_t device_open(char* name);
int device_close(device_t device);
int device_writedata(device_t device, void* data, uint32_t size);
int device_readdata(device_t device, void* buffer, uint32_t size);
int device_command(device_t device, uint32_t command, uint32_t param);

#endif
