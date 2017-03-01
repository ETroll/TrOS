#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include "showcase.h"
#include "syslog.h"
#include "../ui/ui_textbox.h"
#include "../ui/ui_button.h"

static ui_window_t* window = NULL;

static void showcase_inputhandler(ui_message_t code, int val, void* self);

static void btn_ok_clicked();
static void btn_cancel_clicked();

ui_window_t* showcase_create()
{
    window  = ui_window_create("UI Component Showcase");
    window->handlemessage = showcase_inputhandler;

    ui_item_t* btn = ui_button_create(40, 10, 12, "    OK", btn_ok_clicked);
    ui_item_t* btn2 = ui_button_create(54, 10, 12, "  CANCEL", btn_cancel_clicked);

    ui_item_t* tb = ui_textbox_create(2, 2, 36, 22);

    ui_textbox_appendline(tb, "Lorem ipsum dolor sit amet, consectetur \
adipiscing elit, sed do eiusmod tempor incididunt ut labore et \
dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation \
ullamco laboris nisi ut aliquip ex ea commodo consequat.");
    list_add(window->items, btn);
    list_add(window->items, btn2);
    list_add(window->items, tb);

    return window;
}

void showcase_inputhandler(ui_message_t code, int val, void* self)
{
    if(self)
    {
        ui_window_inputhandler(code, val, (ui_window_t*)self);
    }
}

void btn_ok_clicked()
{
    syslog_log(1, SYSLOG_INFO, "OK Clicked");
}

void btn_cancel_clicked()
{
    syslog_log(1, SYSLOG_INFO, "Cancel Clicked");
}
