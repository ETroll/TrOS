#include <Tros/Syscalls.h>
#include <stdio.h>
#include "Terminal.h"

file_t* stdout = NULL;
file_t* stdin = NULL;

//Task 0 - And idle task
void app_init(void)
{
    device_t device = open("uart0");
    if(device > 0)
    {
    		// They will actually point to the variable in the stack for the init process
    		// this is bad, and I should feel bad.
    		stdout = (file_t*)&device;
    		stdin = (file_t*)&device;

    		if(!fork()) terminal_main();
    }
    while(1)
    {
    		yield();
    }
}
