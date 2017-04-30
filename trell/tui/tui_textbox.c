#include <stdlib.h>
#include <string.h>
#include "tui.h"
#include "tui_textbox.h"

typedef struct {
    char* buffer;
    uint32_t bufferpos;
    uint32_t buffersize;
    uint8_t linewidth;
    uint8_t lineoffset; //(For scrolling)
} tui_textbox_t;

static void tui_textbox_paint(tui_context_t* ctx, void* self);
static void tui_textbox_dispose(void* self);

tui_item_t* tui_textbox_create(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    tui_item_t* item = (tui_item_t*)malloc(sizeof(tui_item_t));
    if(item)
    {
        tui_textbox_t* tb = (tui_textbox_t*)malloc(sizeof(tui_textbox_t));
        if(tb)
        {
            tb->buffersize = (width-1) * height;
            tb->linewidth = width-1;
            tb->lineoffset = 0;
            tb->buffer = (char*)malloc(tb->buffersize);
            tb->bufferpos = 0;

            item->handlemessage = NULL;
            item->paint = tui_textbox_paint;
            item->dispose = tui_textbox_dispose;
            item->visible = TRUE;
            item->selectable = FALSE;
            item->backcolor = tui_LIGHT_GRAY;
            item->frontcolor = tui_BLACK;
            item->pos.x = x;
            item->pos.y = y;
            item->pos.width = width;
            item->pos.height = height;
            item->subitems = NULL;//list_create();
            item->content = (void*)tb;
            tui_textbox_clear(item);
        }
    }
    return item;
}

void tui_textbox_dispose(void* self)
{
    if(self)
    {
        tui_item_t* item = (tui_item_t*)self;
        if(item->content)
        {
            tui_textbox_t* tb = (tui_textbox_t*)item->content;
            if(tb->buffer)
            {
                free(tb->buffer);
            }
            free(item->content);
        }
        tui_item_dispose(item);
    }
}

void tui_textbox_appendchar(tui_item_t* tb, char c)
{
    tui_textbox_t* cont = (tui_textbox_t*)tb->content;
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

void tui_textbox_append(tui_item_t* tb, char* text)
{
    //TODO: Create pos marker and append.
    //      Increase buffer 4K at a time.
    //      Not just a width*height like now.
    if(tb->content)
    {
        tui_textbox_t* cont = (tui_textbox_t*)tb->content;
        if(cont->buffer)
        {
            unsigned int size = 0;
            while(text[size] &&
                (cont->bufferpos < cont->buffersize))
            {
                tui_textbox_appendchar(tb, text[size++]);
            }
            cont->buffer[cont->bufferpos] = '\0';
        }
    }
}

void tui_textbox_appendline(tui_item_t* tb, char* text)
{
    tui_textbox_append(tb, text);
    tui_textbox_appendchar(tb, '\n');
}

void tui_textbox_clear(tui_item_t* tb)
{
    if(tb->content)
    {
        tui_textbox_t* cont = (tui_textbox_t*)tb->content;
        if(cont->buffer)
        {
            memset(cont->buffer, 0x00, cont->buffersize);
        }
    }
}

void tui_textbox_paint(tui_context_t* ctx, void* self)
{
    if(self && ctx)
    {
        tui_item_t* item = (tui_item_t*)self;
        if(item->content)
        {
            tui_textbox_t* cont = (tui_textbox_t*)item->content;
            for(uint32_t y = item->pos.y; y < item->pos.height; y++)
            {
                for(uint32_t x = item->pos.x; x < cont->linewidth; x++)
                {
                    tui_cell_t* cell = &ctx->buffer[(y * ctx->width) + x];
                    cell->backcolor = item->backcolor;
                    cell->frontcolor = item->frontcolor;;
                    cell->dirty = TRUE;

                    char data = cont->buffer[(((y-item->pos.y)+cont->lineoffset)*cont->linewidth) + (x-item->pos.x)];

                    cell->data = data;
                }
            }

            for(uint32_t y = item->pos.y; y < item->pos.height; y++)
            {
                tui_cell_t* cell = &ctx->buffer[(y * ctx->width) + item->pos.width];
                cell->backcolor = item->backcolor;
                cell->frontcolor = item->frontcolor;;
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
