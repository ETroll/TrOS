#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include "showcase.h"
#include "syslog.h"
#include "../ui/ui_textbox.h"
#include "../ui/ui_button.h"

static ui_window_t* window = NULL;

void showcase_inputhandler(ui_message_t code, int val);

ui_window_t* showcase_create()
{
    window  = ui_window_create("UI Component Showcase");
    window->handlemessage = showcase_inputhandler;

    return window;
}

void showcase_inputhandler(ui_message_t code, int val)
{
    syslog_log(1, SYSLOG_INFO, "Showcase got code: %x and val: %x", code, val);
}
