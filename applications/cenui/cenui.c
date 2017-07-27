
#include <trlib/device.h>
// #include <trlib/system.h>
#include <trlib/threading.h>
#include <trlib/framebuffer.h>
#include <trlib/mq.h>
#include <stdio.h>
#include <string.h>

file_t* stdout = NULL;
file_t* stdin = NULL;

int main(int argc, char** argv)
{
    // int pid = system_pid();

    device_t vga = device_open("vga1");

    //Later, store the information in a list for later, now we are just using
    //a variable on the stack to test the syscall
    fbmode_information_t fbinfo;

    while(device_command(vga, FB_IOCTL_GETMODES, (uint32_t)&fbinfo) > 0)
    {
        device_command(vga, FB_IOCTL_CHANGEMODE, fbinfo.mode);
        thread_sleep(500);
    }

    //Use first mode for now
    device_command(vga, FB_IOCTL_GETMODES, (uint32_t)&fbinfo);
    device_command(vga, FB_IOCTL_CHANGEMODE, fbinfo.mode);

    // device_command(vga, 1, 0); //Planar 640x480
    // device_command(vga, 1, 1); //Linear 320x200
    // thread_sleep(1000);
    // device_command(vga, 1, 1);
    // thread_sleep(1000);
    // device_command(vga, 1, 0);

    while(TRUE)
    {
        thread_sleep(1000);
    }
    return 0;
}
