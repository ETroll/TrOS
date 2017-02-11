#ifndef INCLUDE_UI_TEXTBOX_H
#define INCLUDE_UI_TEXTBOX_H

ui_item_t* ui_textbox_create(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void ui_textbox_append(ui_item_t* tb, char* text);
void ui_textbox_appendchar(ui_item_t* tb, char c);
void ui_textbox_appendline(ui_item_t* tb, char* text);
void ui_textbox_clear(ui_item_t* tb);


#endif
