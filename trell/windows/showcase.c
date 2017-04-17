#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <trlib/system.h>
#include <trlib/threading.h>
#include "showcase.h"
#include "syslog.h"
#include "../ui/ui_textbox.h"
#include "../ui/ui_button.h"
#include "../ui/ui_label.h"

static ui_window_t* window = NULL;
static ui_item_t* label = NULL;
static ui_item_t* threadlabel = NULL;

static void showcase_inputhandler(ui_message_t code, int val, void* self);

static void btn_ok_clicked();
static void btn_cancel_clicked();
static void btn_execute_clicked();
static void btn_newthread_clicked();

static void showcase_threadloop();

ui_window_t* showcase_create()
{
    window  = ui_window_create("UI Component Showcase");
    window->handlemessage = showcase_inputhandler;

    ui_item_t* btn = ui_button_create(40, 5, 12, "    OK", btn_ok_clicked);
    ui_item_t* btn2 = ui_button_create(54, 5, 12, "  CANCEL", btn_cancel_clicked);
    ui_item_t* btn_newwin = ui_button_create(40, 7, 21, "Create:  New thread", btn_newthread_clicked);
    ui_item_t* btn_execute = ui_button_create(40, 8, 21, "Execute: Tusse", btn_execute_clicked);
    ui_item_t* tb = ui_textbox_create(2, 2, 36, 22);
    label = ui_label_create(40, 2, 24, "This is a label!");

    threadlabel = ui_label_create(40, 3, 24, "Label");
    threadlabel->visible = FALSE;

    ui_textbox_appendline(tb, "Lorem ipsum dolor sit amet, consectetur \
adipiscing elit, sed do eiusmod tempor incididunt ut labore et \
dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation \
ullamco laboris nisi ut aliquip ex ea commodo consequat.");
    list_add(window->items, btn);
    list_add(window->items, btn2);
    list_add(window->items, btn_newwin);
    list_add(window->items, btn_execute);
    list_add(window->items, tb);
    list_add(window->items, label);
    list_add(window->items, threadlabel);

    return window;
}

void showcase_inputhandler(ui_message_t code, int val, void* self)
{
    if(self)
    {
        ui_window_inputhandler(code, val, (ui_window_t*)self);
    }
}

void btn_execute_clicked()
{
    char* argv[] =
    {
        "/fd0/tusse",
        "test",
        NULL
    };
    syslog_log(1, SYSLOG_INFO, "Trying to execute file: %s", argv[0]);
    int retval = system_execute(argv);
    ui_label_set_text(label, "Executed file!");
    syslog_log(1, SYSLOG_INFO, "system_execute returned %d", retval);
}

void btn_newthread_clicked()
{
    threadlabel->visible = TRUE;
    ui_label_set_text(threadlabel, "TEST");
    thread_start(&showcase_threadloop);
}

void btn_ok_clicked()
{
    syslog_log(1, SYSLOG_INFO, "OK Clicked");
    ui_label_set_text(label, "You pressed OK!");
}

void btn_cancel_clicked()
{
    syslog_log(1, SYSLOG_INFO, "Cancel Clicked");
    ui_label_set_text(label, "You pressed CANCEL!");
}

void showcase_threadloop()
{
    uint32_t counter = 0;
    char buffer[20];
    while(1)
    {
        sprintf(buffer, "Thread: %d\0", counter++);
        ui_label_set_text(threadlabel, buffer);
    }
}
