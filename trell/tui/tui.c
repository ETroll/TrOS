#include <trlib/device.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <keycodes.h>

#include "tui.h"

#define FRAME_ROWS 25
#define FRAME_COLS 80

#define IOCTL_VGA_COLOR         1
#define IOCTL_VGA_CURSER_POS    2
#define IOCTL_VGA_TOGGLE_CURSOR 3
#define IOCTL_VGA_CLEAR_MEM     4

#define COLOR(front, back) (back << 4) | front

static void tui_window_paint(tui_window_t* window, tui_context_t* ctx);
static void tui_menubar_paint(tui_desktop_t* desktop);
static void tui_window_inputhandler(tui_event_t code, int val, void* self);

static void tui_context_flush(tui_context_t* context);

static tui_menubar_t* tui_menubar_create();

static tui_menu_t* tui_menu_create(char* text);
// static void tui_menu_add_item(tui_menu_t* menu, char* text);
// static void tui_menu_remove_item(tui_menuitem_t*);
// static void tui_menu_dispose(tui_menu_t*);


tui_context_t* tui_context_create(char* devicename)
{
    tui_context_t* context = NULL;
    device_t vga = device_open(devicename);

    if(vga)
    {
        context = (tui_context_t*)malloc(sizeof(tui_context_t));
        context->device = vga;
        context->width = FRAME_COLS;
        context->height = FRAME_ROWS;
        context->buffer = (tui_cell_t*)malloc(sizeof(tui_cell_t) * FRAME_COLS * FRAME_ROWS);

        device_command(vga, IOCTL_VGA_TOGGLE_CURSOR, 0);
        device_command(vga, IOCTL_VGA_COLOR, COLOR(tui_WHITE, tui_LIGHT_RED));
        device_command(vga, IOCTL_VGA_CLEAR_MEM, 0);


        for(uint32_t i = 0; i<FRAME_COLS * FRAME_ROWS; i++)
        {
            tui_cell_t* cell = &context->buffer[i];
            cell->backcolor = tui_LIGHT_RED;
            cell->frontcolor = tui_WHITE;
            cell->data = 0x00;
            cell->dirty = FALSE;
        }
    }
    return context;
}

tui_desktop_t* tui_desktop_create(tui_context_t* context)
{
    tui_desktop_t* desktop = (tui_desktop_t*)malloc(sizeof(tui_desktop_t));
    if(desktop != NULL)
    {
        desktop->windows = list_create();
        desktop->context = context;
        desktop->activeWindow = NULL;
        desktop->menubar = tui_menubar_create();
        desktop->fillColor = tui_LIGHT_RED;
    }
    return desktop;
}

tui_menubar_t* tui_menubar_create()
{
    return (tui_menubar_t*)malloc(sizeof(tui_menubar_t));
}

tui_window_t* tui_window_create(char* title)
{
    tui_window_t* window = (tui_window_t*)malloc(sizeof(tui_window_t));
    if(window)
    {
        window->title = NULL;
        window->backcolor = tui_LIGHT_GRAY;
        window->frontcolor = tui_BLACK;
        window->menu = tui_menu_create(title);
        window->items = list_create();
        window->activeItem = NULL;
        window->handlemessage = tui_window_inputhandler;

        window->pos.x = 0;
        window->pos.y = 0;
        window->pos.width = FRAME_COLS;
        window->pos.height = FRAME_ROWS-1;

        if(title)
        {
            window->title = (char*)malloc(strlen(title)+1);
            strcpy(window->title, title);
        }
    }
    return window;
}

tui_menu_t* tui_menu_create(char* text)
{
    tui_menu_t* menu = (tui_menu_t*)malloc(sizeof(tui_menu_t));
    if(menu)
    {
        menu->active = 0;
        menu->items = list_create();
        menu->text = NULL;

        if(text)
        {
            menu->text = (char*)malloc(strlen(text)+1);
            strcpy(menu->text, text);
        }
    }
    return menu;
}

void tui_desktop_set_activewindow(tui_desktop_t* desktop,  tui_window_t* window)
{
    if(desktop->activeWindow != NULL)
    {
        desktop->activeWindow->handlemessage(TUI_WINDOW_LEAVE, 0, desktop->activeWindow);
    }
    desktop->activeWindow = window;
    desktop->activeWindow->handlemessage(TUI_WINDOW_ENTER, 0, desktop->activeWindow);
}

void tui_window_inputhandler(tui_event_t code, int val, void* self)
{
    tui_window_t* window = (tui_window_t*)self;
    if(code == TUI_KEYSTROKE)
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
                    tui_item_t* itm = (tui_item_t*)prevItem->data;
                    if(itm->handlemessage)
                    {
                        itm->handlemessage(TUI_ITEM_LOSTFOCUS, 0, itm);
                    }
                }

                if(window->activeItem != NULL)
                {
                    tui_item_t* itm = (tui_item_t*)window->activeItem->data;
                    if(itm->handlemessage)
                    {
                        itm->handlemessage(TUI_ITEM_GOTFOCUS, 0, itm);
                    }
                }
            } break;
            default:
            {
                if(window->activeItem != NULL)
                {
                    tui_item_t* itm = (tui_item_t*)window->activeItem->data;
                    if(itm->handlemessage)
                    {
                        itm->handlemessage(TUI_KEYSTROKE, val, itm);
                    }
                }
            }break;
        }
    }
}


// void tui_menu_dispose(tui_menu_t* menu)
// {
//     if(menu)
//     {
//         if(menu->text)
//         {
//             free(menu->text);
//         }
//         if(menu->items->size > 0)
//         {
//             foreach(i, menu->items)
//             {
//                 tui_menu_remove_item(i->data);
//             }
//             list_free(menu->items);
//         }
//         free(menu);
//     }
// }

// void tui_menu_add_item(tui_menu_t* menu, char* text)
// {
//
// }

// void tui_menu_remove_item(tui_menuitem_t* item)
// {
//     //free up item! (See tui_menu_dispose usage)
// }

void tui_redraw(tui_desktop_t* desktop)
{
    if(desktop->activeWindow != NULL)
    {
        tui_window_paint(desktop->activeWindow, desktop->context);
    }
    else
    {
        for(uint32_t y = 0; y < FRAME_ROWS-1; y++)
        {
            for(uint32_t x = 0; x < FRAME_COLS; x++)
            {
                tui_cell_t* cell = &desktop->context->buffer[y * desktop->context->width + x];
                cell->backcolor = desktop->fillColor;
                cell->frontcolor = tui_BLACK;
                cell->dirty = TRUE;
            }
        }
    }

    if(desktop->menubar != NULL)
    {
        tui_menubar_paint(desktop);
    }
    else
    {
        for(uint32_t x = 0; x < FRAME_COLS; x++)
        {
            tui_cell_t* cell = &desktop->context->buffer[(FRAME_ROWS-1) * desktop->context->width + x];
            cell->backcolor = tui_GREEN;
            cell->frontcolor = tui_BLACK;
            cell->dirty = TRUE;
        }
    }

    tui_context_flush(desktop->context);
}

void tui_window_paint(tui_window_t* window, tui_context_t* ctx)
{
    if(window && ctx)
    {
        uint32_t max_x = FRAME_COLS;
        uint32_t max_y = FRAME_ROWS-1;

        for(uint32_t y = 0; y < max_y; y++)
        {
            for(uint32_t x = 0; x < max_x; x++)
            {
                tui_cell_t* cell = &ctx->buffer[y * ctx->width + x];
                cell->backcolor = window->backcolor;
                cell->frontcolor = window->frontcolor;
                cell->dirty = TRUE;

                if(y == 0 || y == (max_y-1))
                {
                    if(x == 0 && y == 0)
                    {
                        cell->data = 0xC9;
                    }
                    else if(x == 0 && y == (max_y-1))
                    {
                        cell->data = 0xC8;
                    }
                    else if(x == (max_x-1) && y == 0)
                    {
                        cell->data = 0xBB;
                    }
                    else if(x == (max_x-1) &&y == (max_y-1))
                    {
                        cell->data = 0xBC;
                    }
                    else
                    {
                        cell->data = 0xCD;
                    }
                }
                else if(x == 0 || x == (max_x-1))
                {
                    cell->data = 0xBA;
                }
                else
                {
                    cell->data = 0x00;
                }
            }
        }

        if(window->title)
        {
            uint32_t len = strlen(window->title) + 2;
            uint32_t startOffset = ((0 + (FRAME_COLS / 2)) - (len / 2)) - 1;

            for(uint32_t x = startOffset, c = 0; x < (startOffset+len); x++)
            {
                tui_cell_t* cell = &ctx->buffer[x];
                if(x == startOffset)
                {
                    cell->data = 0xB5;
                }
                else if (x == ((startOffset+len)-1))
                {
                    cell->data = 0xC6;
                }
                else
                {
                    cell->data = window->title[c++];
                }
            }
        }

        if(window->items != NULL && window->items->size >0)
        {
            foreach(item, window->items)
            {
                if(((tui_item_t*)item->data)->visible)
                {
                    ((tui_item_t*)item->data)->paint(ctx, item->data);
                }
            }
        }
    }
}

void tui_menubar_paint(tui_desktop_t* desktop)
{
    for(uint32_t x = 0; x < FRAME_COLS; x++)
    {
        tui_cell_t* cell = &desktop->context->buffer[(FRAME_ROWS-1) * desktop->context->width + x];
        cell->backcolor = tui_GREEN;
        cell->frontcolor = tui_BLACK;
        cell->dirty = TRUE;
    }

    uint32_t itemsize = 14;
    uint32_t index = 0;

    foreach(item, desktop->windows)
    {
        tui_window_t* window = (tui_window_t*)item->data;
        if(window)
        {
            char name[14] = "F";
            name[1] = (index+1)+0x30;
            name[2] = ' ';

            if(window->title)
            {
                strncpy(name+3, window->title, 10);
            }
            else
            {
                strncpy(name+3, "----------", 10);
            }

            uint32_t counter = 0;
            tui_cell_color_t backcolor = window == desktop->activeWindow ? tui_LIGHT_GREEN : tui_GREEN;
            for(int x = (index*itemsize); x < (index*itemsize)+(itemsize-1); x++)
            {
                tui_cell_t* cell = &desktop->context->buffer[(FRAME_ROWS-1) * desktop->context->width + x];
                cell->backcolor = backcolor;
                cell->frontcolor = tui_BLACK;
                cell->dirty = TRUE;
                cell->data = name[counter++];
            }
            index++;
        }
    }
}

void tui_context_flush(tui_context_t* context)
{
    for(uint32_t y = 0; y < FRAME_ROWS; y++)
    {
        for(uint32_t x = 0; x < FRAME_COLS; x++)
        {
            tui_cell_t* cell = &context->buffer[y * context->width + x];
            if(cell->dirty)
            {
                char combinedColor = COLOR(cell->frontcolor, cell->backcolor);
                int data = x << 24 | (y & 0xFF) << 16 | ((combinedColor & 0xFF) << 8)
                            | (cell->data & 0xFF);
                device_writedata(context->device, &data, 1);
                cell->dirty = FALSE;
            }
        }
    }
}

void tui_item_dispose(tui_item_t* item)
{
    if(item->subitems && item->subitems->size > 0)
    {
        foreach(i, item->subitems)
        {
            tui_item_t* child = (tui_item_t*)i->data;
            if(child->dispose != NULL)
            {
                child->dispose(child);
            }
        }
        list_free(item->subitems);
    }
    free(item);
}
