
#include <trlib/device.h>
#include <trlib/system.h>
#include <trlib/threading.h>
#include <trlib/trui.h>
#include <trlib/mq.h>
#include <stdio.h>
#include <string.h>
#include <keycodes.h>
#include "tui/tui.h"
#include "tui/list.h"
#include "tui/tui_label.h"
#include "tui/tui_button.h"
#include "windows/syslog.h"
#include "windows/showcase.h"

#define BOCHS_DEBUG __asm__("xchgw %bx, %bx");

// char *builtin_str[] = {
//     "cd",
//     "help"
// };
//
// int (*builtin_func[]) (char **) = {
//     &lsh_cd,
//     &lsh_help
// };

file_t* stdout = NULL;
file_t* stdin = NULL;

tui_desktop_t* desktop = NULL;

static void trell_messageloop();
static void btn_clicked(tui_item_t* item);

int main(int argc, char** argv)
{
    int pid = system_pid();
    tui_context_t* context = tui_context_create("vga0");
    if(context)
    {
        tui_window_t* showcase = showcase_create();
        tui_window_t* syslog = syslog_create();
        desktop = tui_desktop_create(context);

        list_add(desktop->windows, showcase);
        list_add(desktop->windows, syslog);

        tui_desktop_set_activewindow(desktop, showcase);
        tui_redraw(desktop);

        device_t kbd = device_open("kbd");

        int32_t cr3 = system_debug(DEBUG_CR3);
        syslog_log(pid, SYSLOG_INFO, "CR3 %x", cr3);
        syslog_log(pid, SYSLOG_INFO, "Argc %d", argc);

        for(int i = 0; i<argc; i++)
        {
            syslog_log(pid, SYSLOG_INFO, "Argv[%d]: %s", i, argv[i]);
        }

        thread_start(&trell_messageloop);

        while(1)
        {
            int key = 0;
            device_readdata(kbd, &key, 1);

            if(key >= KEY_F1 && key < KEY_F9)
            {
                uint32_t index = key - KEY_F1;
                tui_window_t* win = (tui_window_t*)list_get_at(desktop->windows, index);
                if(win != NULL)
                {
                    tui_desktop_set_activewindow(desktop, win);
                }
                else
                {
                    syslog_log(pid, SYSLOG_INFO, "No window found at index %d", index);
                }
            }
            else
            {
                if(desktop->activeWindow->handlemessage != NULL)
                {
                    desktop->activeWindow->handlemessage(TUI_KEYSTROKE, key, desktop->activeWindow);
                }
            }
            tui_redraw(desktop);
        }
    }

    while(TRUE)
    {
        BOCHS_DEBUG;
    }

    return 0;
}

void trell_messageloop()
{
    trui_clientmessage_t message;
    while(1)
    {
        if(mq_recv(&message, sizeof(trui_clientmessage_t), MQ_NOWAIT) > 0)
        {
            switch (message.command)
            {
                case TRUI_SYSLOG:
                {
                    syslog_log(message.pid, SYSLOG_INFO, message.text);
                } break;
                case TRUI_CREATE_WINDOW:
                {
                    syslog_log(message.pid, SYSLOG_INFO, "Created window: %s", message.text);
                    tui_window_t* window  = tui_window_create(message.text);
                    window->pid = message.pid;
                    list_add(desktop->windows, window);
                    tui_desktop_set_activewindow(desktop, window);

                    trui_servermessage_t responce = {
                        .message = TRUI_WINDOW_CREATED,
                        .param = desktop->windows->size
                    };
                    mq_send(message.pid, &responce, sizeof(trui_servermessage_t), MQ_NOFLAGS);
                    tui_redraw(desktop);
                } break;
                case TRUI_CREATE_LABEL:
                {
                    foreach(win, desktop->windows)
                    {
                        if(((tui_window_t*)win->data)->pid == message.pid)
                        {
                            tui_window_t* window = (tui_window_t*)win->data;
                            tui_item_t* label = tui_label_create(
                                message.x,
                                message.y,
                                message.width,
                                message.text);
                            list_add(window->items, label);
                            break;
                        }
                    }
                } break;
                case TRUI_CREATE_BUTTON:
                {
                    foreach(win, desktop->windows)
                    {
                        if(((tui_window_t*)win->data)->pid == message.pid)
                        {
                            tui_window_t* window = (tui_window_t*)win->data;
                            tui_item_t* button = tui_button_create(
                                message.x,
                                message.y,
                                message.width,
                                message.text,
                                btn_clicked);
                            button->id = message.pid;
                            list_add(window->items, button);
                            break;
                        }
                    }
                }break;
                case TRUI_CLOSE:
                {
                    uint32_t index = 0;
                    foreach(win, desktop->windows)
                    {
                        if(((tui_window_t*)win->data)->pid == message.pid)
                        {
                            tui_window_dispose((tui_window_t*)win->data);
                            tui_window_t* prev = (tui_window_t*)win->prev->data;
                            list_remove_at(desktop->windows, index);
                            tui_desktop_set_activewindow(desktop, prev);
                        }
                        index++;
                    }
                    tui_redraw(desktop);
                }break;
            }
        }
        else
        {
            tui_redraw(desktop);
            thread_sleep(5);
        }
    }
}

void btn_clicked(tui_item_t* item)
{
    //NOTE: This is a ugly hack just to get a simple button "event" back to host process
    //      since Trell does not really do events back to processes yet.
    trui_servermessage_t responce = {
        .message = TRUI_BUTTON_PRESSED,
        .param = 0
    };
    mq_send(item->id, &responce, sizeof(trui_servermessage_t), MQ_NOFLAGS);
}
