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
} ui_cell_color_t;

typedef enum {
    UI_KEYSTROKE,
    UI_WINDOW_ENTER,
    UI_WINDOW_LEAVE,
    UI_ITEM_GOTFOCUS,
    UI_ITEM_LOSTFOCUS
} ui_message_t;

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
} ui_pos_t;

typedef struct {
    void (*handlemessage)(ui_message_t code, int val, void* self);
    void (*paint)(ui_context_t* ctx, void* self);
    void (*dispose)(void* self);
    char visible;
    char selectable;
    list_t* subitems;
    ui_cell_color_t backcolor;
    ui_cell_color_t frontcolor;
    ui_pos_t pos;
    void* content;
} ui_item_t;

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
    char* title;
    ui_menu_t* menu;
    list_t* items;
    list_node_t* activeItem;
    ui_cell_color_t backcolor;
    ui_cell_color_t frontcolor;
    ui_pos_t pos;
    void (*handlemessage)(ui_message_t code, int val, void* self);
} ui_window_t;

typedef struct {
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

ui_window_t* ui_window_create(char* title);
void ui_window_inputhandler(ui_message_t code, int val, ui_window_t* self);

void ui_redraw(ui_desktop_t* desktop);
void ui_desktop_set_activewindow(ui_desktop_t* desktop,  ui_window_t* window);
void ui_item_dispose(ui_item_t* item);

#endif
