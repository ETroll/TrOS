#ifndef TRELL_UI_H
#define TRELL_UI_H

#include <stdint.h>
#include "list.h"

typedef enum  {
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
}ui_cell_color_t;

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
    char selected;
    char* text;
} ui_menuitem_t;

typedef struct {
    char active;
    char* text;
    list_t* items;
} ui_menu_t;

typedef struct {
    ui_context_t* context;
    char* title;
    ui_cell_color_t fillColor;
    ui_menu_t* menu;
} ui_window_t;

typedef struct {
    ui_context_t* context;
    ui_menu_t* windows;
    ui_menu_t* syslog;
} ui_menubar_t;

typedef struct {
    ui_context_t* context;
    list_t* windows;
    ui_window_t* activeWindow;
    ui_menubar_t* menubar;
    ui_cell_color_t fillColor;
} ui_desktop_t;

ui_context_t* ui_context_create(char* devicename);
ui_desktop_t* ui_desktop_create(ui_context_t* context);
ui_window_t* ui_window_create(char* title, ui_context_t* context);


void ui_redraw(ui_desktop_t* desktop);

#endif
