#include <VGA.h>

void vga_move_cursor(unsigned int x, unsigned int y)
{
    asm("nop");
}

void vga_putch(char c)
{
    asm("nop");
}

void vga_puts(char* str)
{
    asm("nop");
}

void vga_clear_screen()
{
    asm("nop");
}

void vga_set_color(unsigned int bg, unsigned int font)
{
    asm("nop");
}

void vga_set_position(unsigned int x, unsigned int y)
{
    asm("nop");
}
