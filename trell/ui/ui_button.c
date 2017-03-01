#include <stdlib.h>
#include <string.h>
#include <keycodes.h>
#include "ui.h"
#include "ui_button.h"
#include "../windows/syslog.h"

typedef struct {
    char* text;
    void (*onclick)();
} ui_button_t;

static void ui_button_paint(ui_context_t* ctx, void* self);
static void ui_button_dispose(void* self);

void ui_button_input(ui_message_t code, int val, void* self);

ui_item_t* ui_button_create(uint8_t x, uint8_t y, uint8_t width, char* text, void (*onclick)())
{
    ui_item_t* item = (ui_item_t*)malloc(sizeof(ui_item_t));
    if(item)
    {
        ui_button_t* btn = (ui_button_t*)malloc(sizeof(ui_button_t));
        if(btn)
        {
            btn->text = (char*)malloc(strlen(text)+1);
            strcpy(btn->text, text);
            btn->onclick = onclick;

            item->handlemessage = ui_button_input;
            item->paint = ui_button_paint;
            item->dispose = ui_button_dispose;
            item->visible = TRUE;
            item->fillColor = UI_LIGHT_GRAY;
            item->pos.x = x;
            item->pos.y = y;
            item->pos.width = width;
            item->pos.height = 1;
            item->subitems = NULL;
            item->content = (void*)btn;
        }
    }
    return item;
}

void ui_button_paint(ui_context_t* ctx, void* self)
{
    if(self && ctx)
    {
        ui_item_t* item = (ui_item_t*)self;
        if(item->content)
        {
            ui_button_t* btn = (ui_button_t*)item->content;


            for(uint32_t x = item->pos.x, strpos = 0;
                x < (item->pos.x + item->pos.width);
                x++)
            {
                ui_cell_t* cell = &ctx->buffer[(item->pos.y * ctx->width) + x];
                cell->backcolor = item->fillColor;
                cell->frontcolor = UI_BLACK;
                cell->dirty = TRUE;

                if(x == item->pos.x)
                {
                    cell->data = '[';
                }
                else if(x == (item->pos.x + item->pos.width)-1)
                {
                    cell->data = ']';
                }
                else
                {
                    if(btn->text[strpos])
                    {
                        cell->data = btn->text[strpos++];
                    }
                    else
                    {
                        cell->data = 0x00;
                    }
                }
            }
        }
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

void ui_button_input(ui_message_t code, int val, void* self)
{
    ui_item_t* item = (ui_item_t*)self;
    switch (code) {
        case UI_ITEM_GOTFOCUS:
            item->fillColor = UI_DARK_GRAY;
        break;
        case UI_ITEM_LOSTFOCUS:
            item->fillColor = UI_LIGHT_GRAY;
        break;
        case UI_KEYSTROKE:
        {
            if(val == KEY_SPACE || val == KEY_RETURN)
            {
                if(item->content)
                {
                    ui_button_t* btn = (ui_button_t*)item->content;
                    if(btn->onclick)
                    {
                        btn->onclick();
                    }
                }
            }
        }break;
        default:
        break;
    }
}
