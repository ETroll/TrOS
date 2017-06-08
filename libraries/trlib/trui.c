#include <string.h>
#include <trlib/trui.h>
#include <trlib/system.h>
#include <trlib/mq.h>
#include <stdarg.h>
#include <stdio.h>

//DISLAIMER:    This "library" was written as a Proof of Concept demo.
//              There are many bad choises done here just to get things Just
//              to work under very controlled circuimstances

trui_window_t trui_create_window(char* title)
{
    //NOTE: Not ask for pid/parent every time!
    int pid = system_pid();
    int parent = system_parentpid();

    trui_servermessage_t responce;
    trui_clientmessage_t message = {
        .command = TRUI_CREATE_WINDOW,
        .pid = pid
    };
    strcpy(message.text, title);

    mq_send(parent, &message, sizeof(trui_clientmessage_t), MQ_NOFLAGS);
    if(mq_recv(&responce, sizeof(trui_servermessage_t), MQ_NOFLAGS) > 0)
    {
        return responce.param;
    }
    else
    {
        return -1; //Error happened
    }
}

trui_label_t trui_create_label(uint8_t x, uint8_t y, uint8_t width, char* text)
{
    //NOTE: Not ask for pid/parent every time!
    int pid = system_pid();
    int parent = system_parentpid();

    trui_clientmessage_t message = {
        .command = TRUI_CREATE_LABEL,
        .pid = pid,
        .width = width,
        .x = x,
        .y = y
    };
    strcpy(message.text, text);

    mq_send(parent, &message, sizeof(trui_clientmessage_t), MQ_NOFLAGS);
    return 0;
}

trui_button_t trui_create_button(uint8_t x, uint8_t y, uint8_t width, char* text)
{
    //NOTE: Not ask for pid/parent every time!
    int pid = system_pid();
    int parent = system_parentpid();

    trui_clientmessage_t message = {
        .command = TRUI_CREATE_BUTTON,
        .pid = pid,
        .width = width,
        .x = x,
        .y = y
    };
    strcpy(message.text, text);

    mq_send(parent, &message, sizeof(trui_clientmessage_t), MQ_NOFLAGS);
    return 0;
}

void trui_syslog_writeline(char* data, ...)
{
    //NOTE: Not ask for pid/parent every time!
    int parent = system_parentpid();
    int pid = system_pid();

    char message[100];
    va_list argptr;
    va_start(argptr, data);
    vsprintf(message, data, argptr);
    va_end(argptr);

    trui_clientmessage_t msg = {
        .command = TRUI_SYSLOG,
        .pid = pid,
    };
    strcpy(msg.text, message);

    mq_send(parent, &msg, sizeof(trui_clientmessage_t), MQ_NOFLAGS);
}

void trui_close()
{
    //NOTE: Not ask for pid/parent every time!
    int pid = system_pid();
    int parent = system_parentpid();
    trui_clientmessage_t message = {
        .command = TRUI_CLOSE,
        .pid = pid
    };
    mq_send(parent, &message, sizeof(trui_clientmessage_t), MQ_NOFLAGS);
    system_exit(1);
}
