#ifndef TRELL_UI_H
#define TRELL_UI_H

#include <stdint.h>

enum ui_cell_color_t {
    UI_BLACK = 0x0,
    UI_BLUE = 0x1,
    UI_GREEN = 0x2,
    UI_CYAN = 0x3,
    UI_RED = 0x4,
    UI_MAGENTA = 0x5,
    UI_BROWN = 0x6,
    UI_LIGHT_GRAY = 0x7,
    UI_DARK_GRAY =0x8,
    UI_LIGHT_BLUE = 0x9,
    UI_LIGHT_GREEN = 0xA,
    UI_LIGHT_CYAN = 0xB,
    UI_LIGHT_RED = 0xC,
    UI_LIGHT_MAGENTA = 0xD,
    UI_YELLOW = 0xE,
    UI_WHITE = 0xF
};

typedef struct {
    char frontcolor:4;
    char backcolor:4;
    char data;
    short dirty:1;
} __attribute__((packed)) ui_cell_t;

typedef struct {
    int32_t device;
    ui_cell_t* buffer;
    uint8_t width;
    uint8_t height;
} ui_context_t;

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
    ui_context_t* context;
    char* title;
} ui_window_t;

ui_context_t* ui_context_create(char* devicename);
ui_window_t* ui_window_create(char* title, uint8_t x, uint8_t y, uint8_t width,
    uint8_t height, ui_context_t* context);
void ui_window_paint(ui_window_t* window);


#endif
