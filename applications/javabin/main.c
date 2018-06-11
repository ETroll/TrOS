#include <stdio.h>
#include <string.h>
#include <trlib/trui.h>
#include <trlib/system.h>
#include <trlib/threading.h>
#include <trlib/mq.h>

file_t* stdout = NULL;
file_t* stdin = NULL;

int main(int argc, char** argv)
{
    int pid = system_pid();
    trui_window_t window = trui_create_window("Hello JavaBin!");
    trui_syslog_writeline("Got WINID: %d", window);

    trui_create_label(2, 2, 50, "This is a simple label!");
    trui_create_button(2, 4, 20, "Close application");

    trui_servermessage_t responce;
    while(mq_recv(&responce, sizeof(trui_servermessage_t), MQ_NOFLAGS))
    {
        if(responce.message == TRUI_BUTTON_PRESSED)
        {
            trui_syslog_writeline("Button %d was pressed. Exiting PID %d", responce.param, pid);
            break;
        }
    }
    trui_close();
    return 0;
}
