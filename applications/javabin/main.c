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
        .command = TRUI_CREATE_WINDOW,
        .pid = pid,
        .text = "Hello JavaBin!\0"
    };

    mq_send(parent, &message, sizeof(trui_clientmessage_t), MQ_NOFLAGS);


    system_exit(1);
    return 0;
}
