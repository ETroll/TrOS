#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include "syslog.h"
#include "../tui/tui_textbox.h"

static tui_window_t* window = NULL;
static tui_item_t* tb = NULL;

void syslog_inputhandler(tui_event_t code, int val, void* self);

tui_window_t* syslog_create()
{
    window  = tui_window_create("System log");
    window->handlemessage = syslog_inputhandler;

    tb = tui_textbox_create(
        window->pos.x + 1,
        window->pos.y + 1,
        window->pos.width - 2,
        window->pos.height - 1);
    list_add(window->items, tb);

    return window;
}

void syslog_log(uint32_t pid, syslog_severity_t sev, char* data, ...)
{
    char message[1024];
    va_list argptr;
    va_start(argptr, data);
    vsprintf(message, data, argptr);
    va_end(argptr);

    char prefix[20];
    sprintf(prefix, "[%d] ", pid);

    tui_textbox_append(tb, prefix);
    tui_textbox_appendline(tb, message);
}


void syslog_inputhandler(tui_event_t code, int val, void* self)
{
    // if(code == tui_KEYSTROKE && tb != NULL)
    // {
    //     //TODO: Just navigation line arrows and pgup/down home/end
    //     tui_textbox_appendchar(tb, (char)(val & 0xFF));
    // }
}
