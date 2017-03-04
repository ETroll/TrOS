#ifndef INCLUDE_UI_LABEL_H
#define INCLUDE_UI_LABEL_H

ui_item_t* ui_label_create(uint8_t x, uint8_t y, uint8_t width, char* text);
void ui_label_set_text(ui_item_t* lbl, char* text);

#endif
