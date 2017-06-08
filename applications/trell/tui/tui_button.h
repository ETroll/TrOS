#ifndef INCLUDE_TUI_BUTTON_H
#define INCLUDE_TUI_BUTTON_H

tui_item_t* tui_button_create(uint8_t x, uint8_t y, uint8_t width, char* text, void (*onclick)(tui_item_t*));


#endif
