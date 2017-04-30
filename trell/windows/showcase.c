#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <trlib/system.h>
#include <trlib/threading.h>
#include "showcase.h"
#include "syslog.h"
#include "../tui/tui_textbox.h"
#include "../tui/tui_button.h"
#include "../tui/tui_label.h"

static tui_window_t* window = NULL;
static tui_item_t* label = NULL;
static tui_item_t* threadlabel = NULL;

static void showcase_inputhandler(tui_event_t code, int val, void* self);

static void btn_ok_clicked();
static void btn_cancel_clicked();
static void btn_execute_clicked();
static void btn_newthread_clicked();

static void showcase_threadloop();

tui_window_t* showcase_create()
{
    window  = tui_window_create("UI Component Showcase");
    window->handlemessage = showcase_inputhandler;

    tui_item_t* btn = tui_button_create(40, 5, 12, "    OK", btn_ok_clicked);
    tui_item_t* btn2 = tui_button_create(54, 5, 12, "  CANCEL", btn_cancel_clicked);
    tui_item_t* btn_newwin = tui_button_create(40, 7, 21, "Create:  New thread", btn_newthread_clicked);
    tui_item_t* btn_execute = tui_button_create(40, 8, 21, "Execute: Tusse", btn_execute_clicked);
    tui_item_t* tb = tui_textbox_create(2, 2, 36, 22);
    label = tui_label_create(40, 2, 24, "This is a label!");

    threadlabel = tui_label_create(40, 3, 24, "Label");
    threadlabel->visible = FALSE;

    tui_textbox_appendline(tb, "Lorem ipsum dolor sit amet, consectetur \
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

void showcase_inputhandler(tui_event_t code, int val, void* self)
{
    if(self)
    {
        tui_window_inputhandler(code, val, (tui_window_t*)self);
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
    tui_label_set_text(label, "Executed file!");
    syslog_log(1, SYSLOG_INFO, "system_execute returned %d", retval);
}

void btn_newthread_clicked()
{
    threadlabel->visible = TRUE;
    tui_label_set_text(threadlabel, "TEST");
    thread_start(&showcase_threadloop);
}

void btn_ok_clicked()
{
    syslog_log(1, SYSLOG_INFO, "OK Clicked");
    tui_label_set_text(label, "You pressed OK!");
}

void btn_cancel_clicked()
{
    syslog_log(1, SYSLOG_INFO, "Cancel Clicked");
    tui_label_set_text(label, "You pressed CANCEL!");
}

void showcase_threadloop()
{
    char buffer[20];
    for(int i = 0; i<10; i++)
    {
        sprintf(buffer, "Thread: %d\0", i);
        tui_label_set_text(threadlabel, buffer);
        thread_sleep(2);
    }
}
