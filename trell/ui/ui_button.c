#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "ui_button.h"

static void ui_button_paint(ui_context_t* ctx, void* self);
static void ui_button_dispose(void* self);

ui_item_t* ui_button_create(uint8_t x, uint8_t y, uint8_t width, char* text)
{
    ui_item_t* item = (ui_item_t*)malloc(sizeof(ui_item_t));
    if(item)
    {
        item->handlemessage = NULL;
        item->paint = ui_button_paint;
        item->dispose = ui_button_dispose;
        item->visible = TRUE;
        item->fillColor = UI_DARK_GRAY;
        item->pos.x = x;
        item->pos.y = y;
        item->pos.width = width;
        item->pos.height = 1;
        item->items = NULL;
        item->content = NULL;
    }
    return item;
}

void ui_button_paint(ui_context_t* ctx, void* self)
{
    if(self && ctx)
    {
        // ui_item_t* item = (ui_item_t*)self;
        // for(uint32_t x = item->pos.x; x < item->pos.width; x++)
        // {
        //     ui_cell_t* cell = &ctx->buffer[(item->pos.y * ctx->width) + x];
        //     cell->backcolor = item->fillColor;
        //     cell->frontcolor = UI_BLACK;
        //     cell->dirty = TRUE;
        //     cell->data = ' ';
        // }
    }
}

void ui_button_dispose(void* self)
{
    if(self)
    {
        ui_item_t* item = (ui_item_t*)self;
        ui_item_dispose(item);
    }
}
