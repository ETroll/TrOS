#ifndef INCLUDE_TRLIB_TRUI_H
#define INCLUDE_TRLIB_TRUI_H

#include <stdint.h>

typedef enum
{
    TRUI_CREATE_WINDOW = 0x00,
    TRUI_CREATE_BUTTON,
    TRUI_CREATE_LABEL,
    TRUI_CREATE_TEXTBOX,
    TRUI_SYSLOG,
    TRUI_CLOSE,
} trui_command_t;

typedef enum
{
    TRUI_BUTTON_PRESSED = 0x00,
    TRUI_WINDOW_CREATED
} trui_message_t;

typedef struct
{
    trui_command_t command;
    uint32_t id;
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;
    char text[100];
} trui_clientmessage_t;

typedef struct
{
    trui_message_t message;
    uint32_t param;
} trui_servermessage_t;


int trui_create_window(char* title);
int trui_create_label(uint8_t x, uint8_t y, uint8_t width, char* text);
int trui_create_button(uint8_t x, uint8_t y, uint8_t width, char* text);
#endif
