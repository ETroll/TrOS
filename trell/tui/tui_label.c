#include <stdlib.h>
#include <string.h>
#include "tui.h"
#include "tui_label.h"

static void tui_label_paint(tui_context_t* ctx, void* self);
static void tui_label_dispose(void* self);

tui_item_t* tui_label_create(uint8_t x, uint8_t y, uint8_t width, char* text)
{
    tui_item_t* item = (tui_item_t*)malloc(sizeof(tui_item_t));
    if(item)
    {
        item->handlemessage = NULL;
        item->paint = tui_label_paint;
        item->dispose = tui_label_dispose;
        item->visible = TRUE;
        item->backcolor = tui_LIGHT_GRAY;
        item->frontcolor = tui_BLACK;
        item->pos.x = x;
        item->pos.y = y;
        item->pos.width = width;
        item->pos.height = 1;
        item->subitems = NULL;
        item->selectable = FALSE;
        item->content = (char*)malloc(strlen(text)+1);
        strcpy(item->content, text);
    }
    return item;
}

void tui_label_set_text(tui_item_t* lbl, char* text)
{
    if(lbl->content)
    {
        free(lbl->content);
    }
    lbl->content = (char*)malloc(strlen(text)+1);
    strcpy(lbl->content, text);
}

void tui_label_paint(tui_context_t* ctx, void* self)
{
    if(self && ctx)
    {
        tui_item_t* item = (tui_item_t*)self;
        if(item->content)
        {
            char* text = (char*)item->content;

            for(uint32_t x = item->pos.x, strpos = 0;
                x < (item->pos.x + item->pos.width);
                x++)
            {
                tui_cell_t* cell = &ctx->buffer[(item->pos.y * ctx->width) + x];
                cell->backcolor = item->backcolor;
                cell->frontcolor = item->frontcolor;
                cell->dirty = TRUE;

                if(text[strpos])
                {
                    cell->data = text[strpos++];
                }
                else
                {
                    cell->data = 0x00;
                }
            }
        }
    }
}

void tui_label_dispose(void* self)
{
    if(self)
    {
        tui_item_t* item = (tui_item_t*)self;
        if(item->content)
        {
            free(item->content);
        }
        tui_item_dispose(item);
    }
}
