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

static void btn_execute_clicked(tui_item_t* item);
static void btn_newthread_clicked(tui_item_t* item);
static void btn_javabin_clicked(tui_item_t* item);

static void showcase_threadloop();

tui_window_t* showcase_create()
{
    window  = tui_window_create("UI Component Showcase");

    tui_item_t* btn_execute = tui_button_create(40, 4, 21, "Execute: Tusse", btn_execute_clicked);
    tui_item_t* btn_javabin = tui_button_create(40, 5, 21, "Execute: Javabin", btn_javabin_clicked);
    tui_item_t* btn_newthread = tui_button_create(40, 6, 21, "Create:  New thread", btn_newthread_clicked);
    tui_item_t* tb = tui_textbox_create(2, 2, 36, 22);
    label = tui_label_create(40, 2, 24, "This is a label!");

    threadlabel = tui_label_create(40, 2, 24, "Label");
    threadlabel->visible = FALSE;

    tui_textbox_appendline(tb, "Lorem ipsum dolor sit amet, consectetur \
adipiscing elit, sed do eiusmod tempor incididunt ut labore et \
dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation \
ullamco laboris nisi ut aliquip ex ea commodo consequat.");
    list_add(window->items, btn_execute);
    list_add(window->items, btn_javabin);
    list_add(window->items, btn_newthread);
    list_add(window->items, tb);
    list_add(window->items, label);
    list_add(window->items, threadlabel);

    return window;
}

void btn_execute_clicked(tui_item_t* item)
{
    char* argv[] =
    {
        "/fd0/tusse",
        "test",
        NULL
    };
    syslog_log(1, SYSLOG_INFO, "Trying to execute file: %s", argv[0]);
    int retval = system_execute(argv);
    tui_label_set_text(label, "Executed tusse!");
    syslog_log(1, SYSLOG_INFO, "system_execute returned %d", retval);
}

void btn_javabin_clicked(tui_item_t* item)
{
    char* argv[] =
    {
        "/fd0/javabin",
        NULL
    };
    syslog_log(1, SYSLOG_INFO, "Trying to execute file: %s", argv[0]);
    int retval = system_execute(argv);
    tui_label_set_text(label, "Executed javabin!");
    syslog_log(1, SYSLOG_INFO, "system_execute returned %d", retval);
}

void btn_newthread_clicked(tui_item_t* item)
{
    threadlabel->visible = TRUE;
    tui_label_set_text(threadlabel, "TEST");
    thread_start(&showcase_threadloop);
}

void showcase_threadloop()
{
    char buffer[20];
    for(int i = 0; i<100; i++)
    {
        sprintf(buffer, "Thread: %d\0", i);
        tui_label_set_text(threadlabel, buffer);
        thread_sleep(2);
    }
}
