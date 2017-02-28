#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <keycodes.h>
#include "showcase.h"
#include "syslog.h"
#include "../ui/ui_textbox.h"
#include "../ui/ui_button.h"

static ui_window_t* window = NULL;

void showcase_inputhandler(ui_message_t code, int val, void* self);

ui_window_t* showcase_create()
{
    window  = ui_window_create("UI Component Showcase");
    window->handlemessage = showcase_inputhandler;

    ui_item_t* btn = ui_button_create(10, 10, 12, "    OK", NULL);
    ui_item_t* btn2 = ui_button_create(24, 10, 12, "  CANCEL", NULL);
    list_add(window->items, btn);
    list_add(window->items, btn2);

    return window;
}

void showcase_inputhandler(ui_message_t code, int val, void* self)
{
    if(self)
    {
        ui_window_t* window = (ui_window_t*)self;
        if(code == UI_KEYSTROKE)
        {
            switch (val) {
                case KEY_TAB:
                {
                    list_node_t* prevItem = window->activeItem;
                    if(window->activeItem == NULL)
                    {
                        window->activeItem = window->items->head;
                    }
                    else
                    {
                        window->activeItem = window->activeItem->next;
                    }

                    if(prevItem != NULL)
                    {
                        ui_item_t* itm = (ui_item_t*)prevItem->data;
                        if(itm->handlemessage)
                        {
                            itm->handlemessage(UI_ITEM_LOSTFOCUS, 0, itm);
                        }
                    }

                    if(window->activeItem != NULL)
                    {
                        ui_item_t* itm = (ui_item_t*)window->activeItem->data;
                        if(itm->handlemessage)
                        {
                            itm->handlemessage(UI_ITEM_GOTFOCUS, 0, itm);
                        }
                    }
                } break;
                default:
                {
                    if(window->activeItem != NULL)
                    {
                        ui_item_t* itm = (ui_item_t*)window->activeItem->data;
                        if(itm->handlemessage)
                        {
                            itm->handlemessage(UI_KEYSTROKE, val, itm);
                        }
                    }
                }break;
            }
        }
        //TODO: Create a generic "fallback" for UI_Window that handle
        //      basic UI navigation so each custon window does not need
        //      to think bout this
    }

    //syslog_log(1, SYSLOG_INFO, "Showcase got code: %x and val: %x", code, val);
}
