#include <stdlib.h>
#include <stdint.h>
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

    // ui_textbox_appendline(tb, "Created a textbox!");
    // ui_textbox_appendline(tb, "Created a Some other!");
    return window;
}

void syslog_log(uint32_t pid, syslog_severity_t sev, char* data)
{
    //TODO: Implement and use sprintf
    ui_textbox_append(tb, "[");
    ui_textbox_appendchar(tb, (char)(pid & 0xFF) + 0x30);
    ui_textbox_append(tb, "-");
    ui_textbox_append(tb, (char*)&sev);
    ui_textbox_append(tb, "] ");
    ui_textbox_appendline(tb, data);
}


void syslog_inputhandler(ui_message_t code, int val)
{
    if(code == UI_KEYSTROKE && tb != NULL)
    {
        //TODO: Just navigation line arrows and pgup/down home/end
        ui_textbox_appendchar(tb, (char)(val & 0xFF));
    }
}
