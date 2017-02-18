#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include "syslog.h"
#include "../ui/ui_textbox.h"

static ui_window_t* window = NULL;
static ui_item_t* tb = NULL;

void syslog_inputhandler(ui_message_t code, int val);

ui_window_t* syslog_create()
{
    window  = ui_window_create("System log");
    window->handlemessage = syslog_inputhandler;

    tb = ui_textbox_create(
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

    ui_textbox_append(tb, prefix);
    ui_textbox_appendline(tb, message);
}


void syslog_inputhandler(ui_message_t code, int val)
{
    // if(code == UI_KEYSTROKE && tb != NULL)
    // {
    //     //TODO: Just navigation line arrows and pgup/down home/end
    //     ui_textbox_appendchar(tb, (char)(val & 0xFF));
    // }
}
