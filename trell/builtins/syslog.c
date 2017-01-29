#include <stdlib.h>
#include <stdint.h>
#include "syslog.h"

static ui_window_t* window = NULL;

void syslog_inputhandler(int keystroke);

ui_window_t* syslog_create()
{
    window  = ui_window_create("System log");
    window->inputhandler = syslog_inputhandler;
    
    return window;
}



void syslog_inputhandler(int keystroke)
{

}
