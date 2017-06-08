#include <stdlib.h>
#include <string.h>
#include <keycodes.h>
#include "tui.h"
#include "tui_button.h"
#include "../windows/syslog.h"

typedef struct {
    char* text;
    void (*onclick)(tui_item_t*);
} tui_button_t;

static void tui_button_paint(tui_context_t* ctx, void* self);
static void tui_button_dispose(void* self);

void tui_button_input(tui_event_t code, int val, void* self);

tui_item_t* tui_button_create(uint8_t x, uint8_t y, uint8_t width, char* text, void (*onclick)(tui_item_t*))
{
    tui_item_t* item = (tui_item_t*)malloc(sizeof(tui_item_t));
    if(item)
    {
        tui_button_t* btn = (tui_button_t*)malloc(sizeof(tui_button_t));
        if(btn)
        {
            btn->text = (char*)malloc(strlen(text)+1);
            strcpy(btn->text, text);
            btn->onclick = onclick;

            item->handlemessage = tui_button_input;
            item->paint = tui_button_paint;
            item->dispose = tui_button_dispose;
            item->visible = TRUE;
            item->selectable = TRUE;
            item->id = 0;
            item->backcolor = tui_LIGHT_GRAY;
            item->frontcolor = tui_BLACK;
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

void tui_button_paint(tui_context_t* ctx, void* self)
{
    if(self && ctx)
    {
        tui_item_t* item = (tui_item_t*)self;
        if(item->content)
        {
            tui_button_t* btn = (tui_button_t*)item->content;

            for(uint32_t x = item->pos.x, strpos = 0;
                x < (item->pos.x + item->pos.width);
                x++)
            {
                tui_cell_t* cell = &ctx->buffer[(item->pos.y * ctx->width) + x];
                cell->backcolor = item->backcolor;
                cell->frontcolor = item->frontcolor;
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

void tui_button_dispose(void* self)
{
    if(self)
    {
        tui_item_t* item = (tui_item_t*)self;
        if(item->content)
        {
            tui_button_t* btn = (tui_button_t*)item->content;
            if(btn->text)
            {
                free(btn->text);
            }
            free(item->content);
        }
        tui_item_dispose(item);
    }
}

void tui_button_input(tui_event_t code, int val, void* self)
{
    tui_item_t* item = (tui_item_t*)self;
    switch (code) {
        case TUI_ITEM_GOTFOCUS:
            item->backcolor = tui_DARK_GRAY;
        break;
        case TUI_ITEM_LOSTFOCUS:
            item->backcolor = tui_LIGHT_GRAY;
        break;
        case TUI_KEYSTROKE:
        {
            if(val == KEY_SPACE || val == KEY_RETURN)
            {
                if(item->content)
                {
                    tui_button_t* btn = (tui_button_t*)item->content;
                    if(btn->onclick)
                    {
                        btn->onclick(item);
                    }
                }
            }
        }break;
        default:
        break;
    }
}
