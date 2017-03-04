#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "ui_label.h"

static void ui_label_paint(ui_context_t* ctx, void* self);
static void ui_label_dispose(void* self);

ui_item_t* ui_label_create(uint8_t x, uint8_t y, uint8_t width, char* text)
{
    ui_item_t* item = (ui_item_t*)malloc(sizeof(ui_item_t));
    if(item)
    {
        item->handlemessage = NULL;
        item->paint = ui_label_paint;
        item->dispose = ui_label_dispose;
        item->visible = TRUE;
        item->backcolor = UI_LIGHT_GRAY;
        item->frontcolor = UI_BLACK;
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

void ui_label_set_text(ui_item_t* lbl, char* text)
{
    if(lbl->content)
    {
        free(lbl->content);
    }
    lbl->content = (char*)malloc(strlen(text)+1);
    strcpy(lbl->content, text);
}

void ui_label_paint(ui_context_t* ctx, void* self)
{
    if(self && ctx)
    {
        ui_item_t* item = (ui_item_t*)self;
        if(item->content)
        {
            char* text = (char*)item->content;

            for(uint32_t x = item->pos.x, strpos = 0;
                x < (item->pos.x + item->pos.width);
                x++)
            {
                ui_cell_t* cell = &ctx->buffer[(item->pos.y * ctx->width) + x];
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

void ui_label_dispose(void* self)
{
    if(self)
    {
        ui_item_t* item = (ui_item_t*)self;
        if(item->content)
        {
            free(item->content);
        }
        ui_item_dispose(item);
    }
}
