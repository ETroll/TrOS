// VGA.h
// Minor VGA implementation

void vga_move_cursor(unsigned int x, unsigned int y);
void vga_putch(char c);
void vga_puts(char* str);
void vga_clear_screen();
void vga_set_color(unsigned int bg, unsigned int font);
void vga_set_position(unsigned int x, unsigned int y);
