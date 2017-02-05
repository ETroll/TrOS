#include <stdlib.h>
#include <stdint.h>
#include "syslog.h"
#include "../ui/ui_textbox.h"

static ui_window_t* window = NULL;

void syslog_inputhandler(ui_message_t code, int val);

ui_window_t* syslog_create()
{
    window  = ui_window_create("System log");
    window->handlemessage = syslog_inputhandler;

    ui_item_t* tb = ui_textbox_create(
        window->pos.x + 1,
        window->pos.y + 1,
        window->pos.width - 2,
        window->pos.height - 1);
    list_add(window->items, tb);

    return window;
}



void syslog_inputhandler(ui_message_t code, int val)
{

}
