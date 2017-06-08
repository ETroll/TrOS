#ifndef INCLUDE_TUI_TEXTBOX_H
#define INCLUDE_TUI_TEXTBOX_H

tui_item_t* tui_textbox_create(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void tui_textbox_append(tui_item_t* tb, char* text);
void tui_textbox_appendchar(tui_item_t* tb, char c);
void tui_textbox_appendline(tui_item_t* tb, char* text);
void tui_textbox_clear(tui_item_t* tb);


#endif
