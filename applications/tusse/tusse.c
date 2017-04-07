#include <stdio.h>
#include <string.h>
#include <trlib/system.h>
#include <trlib/mq.h>

file_t* stdout = NULL;
file_t* stdin = NULL;

int main(int argc, char* argv[])
{
    system_debug(DEBUG_NOP);
    // for(int i = 0; i<argc; i++)
    // {
    //     system_debug((unsigned int)argv[i]);
    // }
    int pid = system_pid();
    int parent = system_parentpid();

    system_debug(pid);
    system_debug(parent);

    char* data = "Hello parent!";

    mq_send(parent, data, strlen(data), 0);

    system_exit(1);
    return 0;
}
