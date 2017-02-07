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

    ui_textbox_appendline(tb, "Created a textbox!");
    return window;
}



void syslog_inputhandler(ui_message_t code, int val)
{
    if(code == UI_KEYSTROKE && tb != NULL)
    {
        char key = (char)val;
        ui_textbox_append(tb, &key);
    }
}
