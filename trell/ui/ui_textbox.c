#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "ui_textbox.h"

typedef struct {
    char* buffer;
    uint32_t bufferpos;
    uint32_t buffersize;
    uint8_t linewidth;
    uint8_t lineoffset; //(For scrolling)
} ui_textbox_t;

static void ui_textbox_paint(ui_context_t* ctx, void* self);
static void ui_textbox_dispose(void* self);

ui_item_t* ui_textbox_create(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    ui_item_t* item = (ui_item_t*)malloc(sizeof(ui_item_t));
    if(item)
    {
        ui_textbox_t* tb = (ui_textbox_t*)malloc(sizeof(ui_textbox_t));
        if(tb)
        {
            tb->buffersize = (width-1) * height;
            tb->linewidth = width-1;
            tb->lineoffset = 0;
            tb->buffer = (char*)malloc(tb->buffersize);
            tb->bufferpos = 0;

            item->handlemessage = NULL;
            item->paint = ui_textbox_paint;
            item->dispose = ui_textbox_dispose;
            item->visible = TRUE;
            item->fillColor = UI_LIGHT_GRAY;
            item->pos.x = x;
            item->pos.y = y;
            item->pos.width = width;
            item->pos.height = height;
            item->items = NULL;//list_create();
            item->content = (void*)tb;
            ui_textbox_clear(item);
        }
    }
    return item;
}

void ui_textbox_dispose(void* self)
{
    if(self)
    {
        ui_item_t* item = (ui_item_t*)self;
        if(item->content)
        {
            ui_textbox_t* tb = (ui_textbox_t*)item->content;
            if(tb->buffer)
            {
                free(tb->buffer);
            }
            free(item->content);
        }
        ui_item_dispose(item);
    }
}

void ui_textbox_appendchar(ui_item_t* tb, char c)
{
    ui_textbox_t* cont = (ui_textbox_t*)tb->content;
    if(c == '\n')
    {
        int lineno = cont->bufferpos / cont->linewidth;
        cont->bufferpos = cont->linewidth * (lineno +1);
    }
    else
    {
        if(c != 0x00)
        {
            cont->buffer[cont->bufferpos++] = c;
        }
    }
}

void ui_textbox_append(ui_item_t* tb, char* text)
{
    //TODO: Create pos marker and append.
    //      Increase buffer 4K at a time.
    //      Not just a width*height like now.
    if(tb->content)
    {
        ui_textbox_t* cont = (ui_textbox_t*)tb->content;
        if(cont->buffer)
        {
            unsigned int size = 0;
            while(text[size] &&
                (cont->bufferpos < cont->buffersize))
            {
                ui_textbox_appendchar(tb, text[size++]);
            }
            cont->buffer[cont->bufferpos] = '\0';
        }
    }
}

void ui_textbox_appendline(ui_item_t* tb, char* text)
{
    ui_textbox_append(tb, text);
    ui_textbox_appendchar(tb, '\n');
}

void ui_textbox_clear(ui_item_t* tb)
{
    if(tb->content)
    {
        ui_textbox_t* cont = (ui_textbox_t*)tb->content;
        if(cont->buffer)
        {
            memset(cont->buffer, 0x00, cont->buffersize);
        }
    }
}

void ui_textbox_paint(ui_context_t* ctx, void* self)
{
    if(self && ctx)
    {
        ui_item_t* item = (ui_item_t*)self;
        if(item->content)
        {
            ui_textbox_t* cont = (ui_textbox_t*)item->content;
            for(uint32_t y = item->pos.y; y < item->pos.height; y++)
            {
                for(uint32_t x = item->pos.x; x < cont->linewidth; x++)
                {
                    ui_cell_t* cell = &ctx->buffer[(y * ctx->width) + x];
                    cell->backcolor = item->fillColor;
                    cell->frontcolor = UI_BLACK;
                    cell->dirty = TRUE;

                    char data = cont->buffer[(((y-item->pos.y)+cont->lineoffset)*cont->linewidth) + (x-item->pos.x)];

                    cell->data = data;
                }
            }

            for(uint32_t y = item->pos.y; y < item->pos.height; y++)
            {
                ui_cell_t* cell = &ctx->buffer[(y * ctx->width) + item->pos.width];
                cell->backcolor = item->fillColor;
                cell->frontcolor = UI_BLACK;
                cell->dirty = TRUE;

                if(y == item->pos.y)
                {
                    cell->data = 0x1E;
                }
                else if(y == item->pos.height-1)
                {
                    cell->data = 0x1F;
                }
                else
                {
                    cell->data = 0xB0;
                }
            }
        }
    }
}
