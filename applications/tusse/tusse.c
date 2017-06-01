#include <stdio.h>
#include <string.h>
#include <trlib/system.h>
#include <trlib/mq.h>
#include <trlib/trui.h>

file_t* stdout = NULL;
file_t* stdin = NULL;

int main(int argc, char** argv)
{
    int parent = system_parentpid();
    int pid = system_pid();

    trui_clientmessage_t message = {
        .command = TRUI_SYSLOG,
        .pid = pid
    };

    strcpy(message.text, "Hello parent!\0");
    mq_send(parent, &message, sizeof(trui_clientmessage_t), MQ_NOFLAGS);
    if(argc > 0)
    {
        sprintf(message.text, "Got %d arguments\0", argc);
        mq_send(parent, &message, sizeof(trui_clientmessage_t), MQ_NOFLAGS);

        for(int i = 0; i<argc; i++)
        {
            sprintf(message.text, "Arg%d: %s\0", i, argv[i]);
            mq_send(parent, &message, sizeof(trui_clientmessage_t), MQ_NOFLAGS);
        }
    }
    else
    {
        // char* error = "NO ARGUMENTS!\0";
        strcpy(message.text, "NO ARGUMENTS!\0");
        mq_send(parent, &message, sizeof(trui_clientmessage_t), MQ_NOFLAGS);

    }

    system_exit(1);
    return 0;
}
