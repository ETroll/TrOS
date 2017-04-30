#ifndef INCLUDE_TUI_LABEL_H
#define INCLUDE_TUI_LABEL_H

tui_item_t* tui_label_create(uint8_t x, uint8_t y, uint8_t width, char* text);
void tui_label_set_text(tui_item_t* lbl, char* text);

#endif
