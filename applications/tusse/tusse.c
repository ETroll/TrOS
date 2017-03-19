#include <stdio.h>
#include <syscall.h>

file_t* stdout = NULL;
file_t* stdin = NULL;

int main(int argc, char* argv[])
{
    syscall_debug(0xAABBCCDD);
    // for(int i = 0; i<argc; i++)
    // {
    //     syscall_debug((unsigned int)argv[i]);
    // }
    while(1);
    syscall_exit(1);
    return 0;
}
