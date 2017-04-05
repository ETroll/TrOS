#include <stdio.h>
#include <syscall.h>
#include <string.h>

file_t* stdout = NULL;
file_t* stdin = NULL;

int main(int argc, char* argv[])
{
    syscall_debug(0xAABBCCDD);
    // for(int i = 0; i<argc; i++)
    // {
    //     syscall_debug((unsigned int)argv[i]);
    // }
    int pid = syscall_getpid();
    int parent = syscall_getparentpid();

    syscall_debug(pid);
    syscall_debug(parent);

    char* data = "Hello parent!";

    syscall_sendmessage(parent, data, strlen(data), 0);

    syscall_exit(1);
    return 0;
}
