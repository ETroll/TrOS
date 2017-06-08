#ifndef TRELL_TUI_H
#define TRELL_TUI_H

#include <stdint.h>
#include "list.h"

typedef enum  {
    tui_BLACK = 0x0,
    tui_BLUE = 0x1,
    tui_GREEN = 0x2,
    tui_CYAN = 0x3,
    tui_RED = 0x4,
    tui_MAGENTA = 0x5,
    tui_BROWN = 0x6,
    tui_LIGHT_GRAY = 0x7,
    tui_DARK_GRAY =0x8,
    tui_LIGHT_BLUE = 0x9,
    tui_LIGHT_GREEN = 0xA,
    tui_LIGHT_CYAN = 0xB,
    tui_LIGHT_RED = 0xC,
    tui_LIGHT_MAGENTA = 0xD,
    tui_YELLOW = 0xE,
    tui_WHITE = 0xF
} tui_cell_color_t;

typedef enum {
    TUI_KEYSTROKE,
    TUI_WINDOW_ENTER,
    TUI_WINDOW_LEAVE,
    TUI_ITEM_GOTFOCUS,
    TUI_ITEM_LOSTFOCUS
} tui_event_t;

typedef struct {
    char frontcolor:4;
    char backcolor:4;
    char data;
    short dirty:1;
} __attribute__((packed)) tui_cell_t;

typedef struct {
    int32_t device;
    tui_cell_t* buffer;
    uint8_t width;
    uint8_t height;
} tui_context_t;

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
} tui_pos_t;

typedef struct {
    void (*handlemessage)(tui_event_t code, int val, void* self);
    void (*paint)(tui_context_t* ctx, void* self);
    void (*dispose)(void* self);
    char visible;
    char selectable;
    uint32_t id;
    list_t* subitems;
    tui_cell_color_t backcolor;
    tui_cell_color_t frontcolor;
    tui_pos_t pos;
    void* content;
} tui_item_t;

typedef struct {
    char selected;
    char* text;
} tui_menuitem_t;

typedef struct {
    char active;
    char* text;
    list_t* items;
} tui_menu_t;

typedef struct {
    char* title;
    tui_menu_t* menu;
    list_t* items;
    list_node_t* activeItem;
    tui_cell_color_t backcolor;
    tui_cell_color_t frontcolor;
    tui_pos_t pos;
    uint32_t pid;
    void (*handlemessage)(tui_event_t code, int val, void* self);
} tui_window_t;

typedef struct {
    tui_menu_t* windows;
    tui_menu_t* syslog;
} tui_menubar_t;

typedef struct {
    tui_context_t* context;
    list_t* windows;
    tui_window_t* activeWindow;
    tui_menubar_t* menubar;
    tui_cell_color_t fillColor;
} tui_desktop_t;

tui_context_t* tui_context_create(char* devicename);
tui_desktop_t* tui_desktop_create(tui_context_t* context);

tui_window_t* tui_window_create(char* title);
void tui_window_dispose(tui_window_t* item);


void tui_redraw(tui_desktop_t* desktop);
void tui_desktop_set_activewindow(tui_desktop_t* desktop,  tui_window_t* window);
void tui_item_dispose(tui_item_t* item);

#endif
