#include <stdio.h>
#include <string.h>
#include <trlib/trui.h>

file_t* stdout = NULL;
file_t* stdin = NULL;

int main(int argc, char** argv)
{
    trui_window_t window = trui_create_window("Hello JavaBin!");
    trui_syslog_writeline("Got WINID: %d", window);




    trui_close();
    return 0;
}
