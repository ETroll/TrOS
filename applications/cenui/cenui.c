
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
uint32_t fbline_bytes = 0;
uint32_t fbsize_bytes = 0;
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
    // device_command(vga, FB_IOCTL_GETMODES, (uint32_t)&fbinfo);
    // device_command(vga, FB_IOCTL_CHANGEMODE, fbinfo.mode);

    //Drawing "initalize" stuff
    //Creating for a maximum of 8 bits per pixel.
    //      for now... ;)
    if(fbinfo.width % 16 == 0)
    {
        if(fbinfo.bpp == 8)
        {
            fbline_bytes = fbinfo.width;
            fbppb = 1;
        }
        else if(fbinfo.bpp == 4)
        {
            fbline_bytes = fbinfo.width / 2;
            fbppb = 2;
        }
        else
        {
            //Not a recognised mode!
            //TODO: Send to syslog and exit
            return -1;
        }
        fbsize_bytes = fbline_bytes*fbinfo.height*sizeof(uint8_t);
        framebuffer = (uint8_t*)malloc(fbsize_bytes);
        memset(framebuffer, 0x00, fbsize_bytes);


        draw_rectangle(100, 100, 100, 100, 0x3F);
        framebuffer_swapbuffer(vga, framebuffer, fbsize_bytes);

        while(TRUE)
        {
            thread_sleep(1000);
        }
        return 0;
    }
    else
    {
        //TODO: Send to syslog about bad resolution
        return -1;
    }
}

void draw_pixel(int32_t x, int32_t y, uint8_t c)
{
    uint32_t offset = (y * fbline_bytes) + (x / fbppb);
    //uint32_t offset = (y<<8) + (y<<6) + x;

    if(fbppb == 2)
    {
        if((x % 2) == 0)
        {
            c <<= 4;
            c |= 0x0F;
        }
        else
        {
            c |= 0xF0;
        }
    }
    framebuffer[offset] = c;
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
