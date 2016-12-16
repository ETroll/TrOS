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

static void ui_context_flush(ui_context_t* context);

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

ui_window_t* ui_window_create(char* title, uint8_t x, uint8_t y, uint8_t width,
    uint8_t height, ui_context_t* context)
{
    ui_window_t* window = (ui_window_t*)malloc(sizeof(ui_window_t));
    if(window)
    {
        window->x = x;
        window->y = y;
        window->width = width;
        window->height = height;
        window->context = context;
        window->title = NULL;

        if(title)
        {
            window->title = (char*)malloc(strlen(title)+1);
            strcpy(window->title, title);
        }
    }
    return window;
}

void ui_window_paint(ui_window_t* window)
{
    if(window)
    {
        uint32_t max_x = window->x + window->width;
        uint32_t max_y = window->y + window->height;

        if(window->context)
        {
            if(max_x > window->context->width)
            {
                max_x = window->context->width;
            }
            if(max_y > window->context->height)
            {
                max_y = window->context->height;
            }
            for(uint32_t y = window->y; y < max_y; y++)
            {
                for(uint32_t x = window->x; x < max_x; x++)
                {
                    ui_cell_t* cell = &window->context->buffer[y * window->context->width + x];
                    cell->backcolor = UI_LIGHT_GRAY;
                    cell->frontcolor = UI_BLACK;
                    cell->dirty = TRUE;

                    if(y == window->y || y == (max_y-1))
                    {
                        if(x == window->x && y == window->y)
                        {
                            cell->data = 0xC9;
                        }
                        else if(x == window->x && y == (max_y-1))
                        {
                            cell->data = 0xC8;
                        }
                        else if(x == (max_x-1) && y == window->y)
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
                    else if(x == window->x || x == (max_x-1))
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
                uint32_t startOffset = ((window->y + (window->width / 2)) - (len / 2)) - 1;

                for(uint32_t x = startOffset, c = 0; x < (startOffset+len); x++)
                {
                    ui_cell_t* cell = &window->context->buffer[window->y * window->context->width + x];
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

            ui_context_flush(window->context);
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
