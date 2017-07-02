
#include <trlib/device.h>
#include <trlib/system.h>
#include <trlib/threading.h>
#include <trlib/mq.h>
#include <stdio.h>
#include <string.h>

file_t* stdout = NULL;
file_t* stdin = NULL;

int main(int argc, char** argv)
{
    int pid = system_pid();

    device_t vga = device_open("vga1");

    device_command(vga, 1, 0);
    thread_sleep(1000);
    device_command(vga, 1, 1);
    thread_sleep(1000);
    device_command(vga, 1, 0);

    while(TRUE)
    {
        thread_sleep(10);
    }
    return 0;
}
