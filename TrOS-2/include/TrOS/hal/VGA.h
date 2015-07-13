// VGA.h
// Minor VGA implementation

#ifndef INCLUDE_VGA_H
#define INCLUDE_VGA_H

#define VGA_COLS 80
#define VGA_LINES 25

typedef enum vga_color
{
    VGA_BLACK = 0,
    VGA_BLUE = 1,
    VGA_GREEN = 2,
    VGA_CYAN = 3,
    VGA_RED = 4,
    VGA_MAGENTA = 5,
    VGA_BROWN = 6,
    VGA_LIGHT_GRAY = 7,
    VGA_DARK_GRAY = 8,
    VGA_LIGHT_BLUE = 9,
    VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN = 11,
    VGA_LIGHT_RED = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_YELLOW = 14,
    VGA_WHITE = 15
} vga_color_t;

typedef struct
{
    vga_color_t bg;
    vga_color_t font;
} vga_char_attrib_t;

typedef struct
{
    unsigned int x;
    unsigned int y;
} vga_position_t;

void vga_move_cursor(unsigned int x, unsigned int y);
void vga_putch(char c);
void vga_puts(const char* str);
void vga_clear_screen(vga_char_attrib_t* c);
void vga_set_position(unsigned int x, unsigned int y);
void vga_set_color(vga_char_attrib_t* c);
vga_position_t vga_get_position();


#endif
