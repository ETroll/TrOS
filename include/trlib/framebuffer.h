#ifndef INCLUDE_TRLIB_FRAMEBUFFER_H
#define INCLUDE_TRLIB_FRAMEBUFFER_H

#include <stdint.h>

typedef struct
{
    uint32_t width;
    uint32_t height;
    uint32_t bpp;       //if zero, then text-mode
    uint32_t mode;
} fbmode_information_t;

typedef enum
{
    FB_IOCTL_CHANGEMODE = 0x00,
    FB_IOCTL_GETMODES
} framebuffer_ioctl_t;


void framebuffer_swapbuffer(unsigned int device, unsigned char* buffer, unsigned int length);

#endif
