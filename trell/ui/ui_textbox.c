#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "ui_textbox.h"

typedef struct {
    char* buffer;
    uint32_t bufferpos;
} ui_textbox_t;

// static void handlemessage(ui_message_t code, int val);
static void paint(ui_context_t* ctx, void* self);
static void ui_textbox_dispose(void* self);

ui_item_t* ui_textbox_create(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    ui_item_t* item = (ui_item_t*)malloc(sizeof(ui_item_t));
    if(item)
    {
        ui_textbox_t* tb = (ui_textbox_t*)malloc(sizeof(ui_textbox_t));
        if(tb)
        {
            tb->buffer = (char*)malloc((width-1)*height);
            tb->bufferpos = 0;
            item->handlemessage = NULL;
            item->paint = paint;
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
        if(item->items && item->items->size > 0)
        {
            foreach(i, item->items)
            {
                ui_item_t* child = (ui_item_t*)i->data;
                if(child->dispose != NULL)
                {
                    child->dispose(child);
                }
            }
            list_free(item->items);
        }
        if(item->content)
        {
            ui_textbox_t* tb = (ui_textbox_t*)item->content;
            if(tb->buffer)
            {
                free(tb->buffer);
            }
            free(item->content);
        }
        free(item);
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
            while(text[cont->bufferpos] &&
                (cont->bufferpos < ((tb->pos.width-1) * tb->pos.height)))
            {
                if(text[cont->bufferpos] == '\n')
                {
                    int lineno = cont->bufferpos / (tb->pos.width-1);
                    cont->bufferpos = (tb->pos.width-1) * (lineno +1);
                }
                else
                {
                    cont->buffer[cont->bufferpos] = text[cont->bufferpos];
                    cont->bufferpos++;
                }
            }
            cont->buffer[cont->bufferpos] = '\0';
        }
    }
}

void ui_textbox_appendline(ui_item_t* tb, char* text)
{
    ui_textbox_append(tb, text);
    ui_textbox_append(tb, "\n");
}

void ui_textbox_clear(ui_item_t* tb)
{
    if(tb->content)
    {
        ui_textbox_t* cont = (ui_textbox_t*)tb->content;
        if(cont->buffer)
        {
            memset(cont->buffer, 0x00, (tb->pos.width-1) * tb->pos.height);
        }
    }
}

// void handlemessage(ui_message_t code, int val)
// {
//
// }

void paint(ui_context_t* ctx, void* self)
{
    if(self && ctx)
    {
        ui_item_t* item = (ui_item_t*)self;

        uint32_t cy = 0;
        for(uint32_t y = item->pos.y; y < item->pos.height; y++, cy++)
        {
            uint32_t cx = 0;
            for(uint32_t x = item->pos.x; x < (item->pos.width-1); x++, cx++)
            {
                ui_cell_t* cell = &ctx->buffer[(y * ctx->width) + x];
                cell->backcolor = item->fillColor;
                cell->frontcolor = UI_BLACK;
                cell->dirty = TRUE;
                cell->data = ((char*)item->content)[(cy * (item->pos.width-1)) + cx];
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
            else if(y == (item->pos.height-1))
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
