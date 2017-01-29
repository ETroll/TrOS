#include <stdlib.h>
#include "ui.h"

static void handlemessage(int code, int value);
static void paint(ui_context_t* ctx, void* self);
static void ui_textbox_dispose(void* self);

ui_item_t* ui_textbox_create(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    ui_item_t* item = (ui_item_t*)malloc(sizeof(ui_item_t));
    if(item)
    {
        item->message = handlemessage;
        item->paint = paint;
        item->dispose = ui_textbox_dispose;
        item->visible = TRUE;
        item->fillColor = UI_LIGHT_RED;
        item->pos.x = x;
        item->pos.y = y;
        item->pos.width = width;
        item->pos.height = height;
        item->items = list_create();
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
        free(item);
    }
}

void handlemessage(int code, int value)
{

}

void paint(ui_context_t* ctx, void* self)
{
    if(self && ctx)
    {
        ui_item_t* item = (ui_item_t*)self;
    }
}
