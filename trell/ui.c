#include <syscall.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ui.h"

#define FRAME_ROWS 25
#define FRAME_COLS 80

#define IOCTL_VGA_COLOR         1
#define IOCTL_VGA_CURSER_POS    2
#define IOCTL_VGA_TOGGLE_CURSOR 3
#define IOCTL_VGA_CLEAR_MEM     4

#define COLOR(front, back) (back << 4) | front

static void ui_window_paint(ui_window_t* window);
static void ui_menubar_paint(ui_desktop_t* desktop);

static void ui_context_flush(ui_context_t* context);

static ui_menubar_t* ui_menubar_create(ui_context_t* context);

static ui_menu_t* ui_menu_create(char* text);
static void ui_menu_add_item(ui_menu_t* menu, char* text);
static void ui_menu_remove_item(ui_menuitem_t*);
static void ui_menu_dispose(ui_menu_t*);


ui_context_t* ui_context_create(char* devicename)
{
    ui_context_t* context = NULL;
    int32_t vga = syscall_opendevice("vga");

    if(vga)
    {
        context = (ui_context_t*)malloc(sizeof(ui_context_t));
        context->device = vga;
        context->width = FRAME_COLS;
        context->height = FRAME_ROWS;
        context->buffer = (ui_cell_t*)malloc(sizeof(ui_cell_t) * FRAME_COLS * FRAME_ROWS);

        syscall_ioctl(vga, IOCTL_VGA_TOGGLE_CURSOR, 0);
        syscall_ioctl(vga, IOCTL_VGA_COLOR, COLOR(UI_WHITE, UI_LIGHT_RED));
        syscall_ioctl(vga, IOCTL_VGA_CLEAR_MEM, 0);


        for(uint32_t i = 0; i<FRAME_COLS * FRAME_ROWS; i++)
        {
            ui_cell_t* cell = &context->buffer[i];
            cell->backcolor = UI_LIGHT_RED;
            cell->frontcolor = UI_WHITE;
            cell->data = 0x00;
            cell->dirty = FALSE;
        }
    }
    return context;
}

ui_desktop_t* ui_desktop_create(ui_context_t* context)
{
    ui_desktop_t* desktop = (ui_desktop_t*)malloc(sizeof(ui_desktop_t));
    if(desktop != NULL)
    {
        desktop->windows = list_create();
        desktop->context = context;
        desktop->activeWindow = NULL;
        desktop->menubar = ui_menubar_create(context);
        desktop->fillColor = UI_LIGHT_RED;
    }
    return desktop;
}

ui_menubar_t* ui_menubar_create(ui_context_t* context)
{
    ui_menubar_t* menubar = (ui_menubar_t*)malloc(sizeof(ui_menubar_t));
    if(menubar != NULL)
    {
        menubar->context = context;
    }
    return menubar;
}

ui_window_t* ui_window_create(char* title, ui_context_t* context)
{
    ui_window_t* window = (ui_window_t*)malloc(sizeof(ui_window_t));
    if(window)
    {
        window->context = context;
        window->title = NULL;
        window->fillColor = UI_LIGHT_GRAY;
        window->menu = ui_menu_create(title);

        if(title)
        {
            window->title = (char*)malloc(strlen(title)+1);
            strcpy(window->title, title);
        }
    }
    return window;
}

ui_menu_t* ui_menu_create(char* text)
{
    ui_menu_t* menu = (ui_menu_t*)malloc(sizeof(ui_menu_t));
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

void ui_menu_dispose(ui_menu_t* menu)
{
    if(menu)
    {
        if(menu->text)
        {
            free(menu->text);
        }
        if(menu->items->size > 0)
        {
            foreach(i, menu->items)
            {
                ui_menu_remove_item(i->data);
            }
            list_free(menu->items);
        }
        free(menu);
    }
}

void ui_menu_add_item(ui_menu_t* menu, char* text)
{

}

void ui_menu_remove_item(ui_menuitem_t* item)
{
    //free up item! (See ui_menu_dispose usage)
}

void ui_redraw(ui_desktop_t* desktop)
{
    if(desktop->activeWindow != NULL)
    {
        ui_window_paint(desktop->activeWindow);
    }
    else
    {
        for(uint32_t y = 0; y < FRAME_ROWS-1; y++)
        {
            for(uint32_t x = 0; x < FRAME_COLS; x++)
            {
                ui_cell_t* cell = &desktop->context->buffer[y * desktop->context->width + x];
                cell->backcolor = desktop->fillColor;
                cell->frontcolor = UI_BLACK;
                cell->dirty = TRUE;
            }
        }
    }

    if(desktop->menubar != NULL)
    {
        ui_menubar_paint(desktop);
    }
    else
    {
        for(uint32_t x = 0; x < FRAME_COLS; x++)
        {
            ui_cell_t* cell = &desktop->context->buffer[(FRAME_ROWS-1) * desktop->context->width + x];
            cell->backcolor = UI_GREEN;
            cell->frontcolor = UI_BLACK;
            cell->dirty = TRUE;
        }
    }

    ui_context_flush(desktop->context);
}

void ui_window_paint(ui_window_t* window)
{
    if(window)
    {
        uint32_t max_x = FRAME_COLS;
        uint32_t max_y = FRAME_ROWS-1;

        if(window->context)
        {
            for(uint32_t y = 0; y < max_y; y++)
            {
                for(uint32_t x = 0; x < max_x; x++)
                {
                    ui_cell_t* cell = &window->context->buffer[y * window->context->width + x];
                    cell->backcolor = UI_LIGHT_GRAY;
                    cell->frontcolor = UI_BLACK;
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
                    ui_cell_t* cell = &window->context->buffer[0 * window->context->width + x];
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
        }
    }
}

void ui_menubar_paint(ui_desktop_t* desktop)
{
    for(uint32_t x = 0; x < FRAME_COLS; x++)
    {
        ui_cell_t* cell = &desktop->context->buffer[(FRAME_ROWS-1) * desktop->context->width + x];
        cell->backcolor = UI_GREEN;
        cell->frontcolor = UI_BLACK;
        cell->dirty = TRUE;
    }

    uint32_t itemsize = 11;

    for(int i = 0; i< 3; i++)
    {
        char name[11] = "F";
        name[1] = (i+1)+0x30;
        name[2] = ' ';

        switch (i)
        {
            case 0:
            {
                if(desktop->activeWindow)
                {
                    strncpy(name+3, desktop->activeWindow->title, 7);
                }
                else
                {
                    strncpy(name+3, "-------", 7);
                }
            } break;
            case 1:
                strncpy(name+3, "Windows", 7);
                break;
            case 2:
                strncpy(name+3, "Syslog", 7);
                break;
        }

        uint32_t counter = 0;
        for(int x = (i*itemsize); x < (i*itemsize)+(itemsize-1); x++)
        {
            ui_cell_t* cell = &desktop->context->buffer[(FRAME_ROWS-1) * desktop->context->width + x];
            cell->backcolor = UI_GREEN;
            cell->frontcolor = UI_BLACK;
            cell->dirty = TRUE;
            cell->data = name[counter++];
        }
    }
}

void ui_context_flush(ui_context_t* context)
{
    for(uint32_t y = 0; y < FRAME_ROWS; y++)
    {
        for(uint32_t x = 0; x < FRAME_COLS; x++)
        {
            ui_cell_t* cell = &context->buffer[y * context->width + x];
            if(cell->dirty)
            {
                char combinedColor = COLOR(cell->frontcolor, cell->backcolor);
                int data = x << 24 | y << 16 | (combinedColor << 8)
                             | (cell->data & 0xFF);
                syscall_writedevice(context->device, &data, 1);
                cell->dirty = FALSE;
            }
        }
    }
}
