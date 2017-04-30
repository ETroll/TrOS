#include <stdio.h>
#include <string.h>
#include <trlib/system.h>
#include <trlib/mq.h>

file_t* stdout = NULL;
file_t* stdin = NULL;

int main(int argc, char** argv)
{
    int parent = system_parentpid();

    char* data = "Hello parent!\0";
    mq_send(parent, data, strlen(data), MQ_NOFLAGS);
    if(argc > 0)
    {
        char buffer[20];
        sprintf(buffer, "Got %d arguments\0", argc);
        mq_send(parent, buffer, strlen(buffer), MQ_NOFLAGS);

        for(int i = 0; i<argc; i++)
        {
            sprintf(buffer, "Arg%d: %s\0", i, argv[i]);
            mq_send(parent, buffer, strlen(buffer), MQ_NOFLAGS);
        }
    }
    else
    {
        char* error = "NO ARGUMENTS!\0";
        mq_send(parent, error, strlen(error), MQ_NOFLAGS);
    }

    system_exit(1);
    return 0;
}
