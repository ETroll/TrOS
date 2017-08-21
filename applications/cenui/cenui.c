
#include <trlib/device.h>
// #include <trlib/system.h>
#include <trlib/threading.h>
#include <trlib/framebuffer.h>
#include <trlib/mq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

file_t* stdout = NULL;
file_t* stdin = NULL;

fbmode_information_t fbinfo = {0};
uint8_t* framebuffer = NULL;
uint32_t fblinesize = 0;                // Wb
uint32_t fbsize = 0;
uint32_t fbppb = 0;                     //pixels per byte

void draw_pixel(int32_t x, int32_t y, uint8_t c);
void draw_rectangle(int32_t x, int32_t y, int32_t height, int32_t width, uint8_t color);

int main(int argc, char** argv)
{
    // int pid = system_pid();

    device_t vga = device_open("vga1");

    //Later, store the information in a list for later, now we are just using
    //a variable on the stack to test the syscall
    while(device_command(vga, FB_IOCTL_GETMODES, (uint32_t)&fbinfo) > 0)
    {
        device_command(vga, FB_IOCTL_CHANGEMODE, fbinfo.mode);
        thread_sleep(500);
    }

    //Use first mode for now
    device_command(vga, FB_IOCTL_GETMODES, (uint32_t)&fbinfo);
    device_command(vga, FB_IOCTL_CHANGEMODE, fbinfo.mode);

    //Drawing "initalize" stuff
    fblinesize = (fbinfo.width * fbinfo.bpp) / 8;
    fbppb = 8 / fbinfo.bpp;

    fbsize = fblinesize*fbinfo.height*sizeof(uint8_t);
    framebuffer = (uint8_t*)malloc(fbsize);

    draw_rectangle(100, 100, 50, 100, 0x2);
    framebuffer_swapbuffer(vga, framebuffer, fbsize);

    while(TRUE)
    {
        thread_sleep(1000);
    }
    return 0;
}

void draw_pixel(int32_t x, int32_t y, uint8_t c)
{
    uint32_t offset = (y * fblinesize) + (x/fbppb);

    if((x % fbppb) == 0)
    {
        c <<= fbinfo.bpp;
        c |= 0x0F;
    }
    else
    {
        c |= 0xF0;
    }

    framebuffer[offset] &= c;
}

void draw_rectangle(int32_t xstart, int32_t ystart, int32_t height, int32_t width, uint8_t color)
{
    for(int x=xstart; x<(xstart+width); x++)
    {
        for(int y=ystart; y<(ystart+height); y++)
        {
            draw_pixel(x,y, color);
        }
    }
}
