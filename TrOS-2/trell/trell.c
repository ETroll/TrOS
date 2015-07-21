// Trell - TrOS default shell

#include <trell/trell.h>
#include <tros/hal/VGA.h>

#define TRELL_RCOLUMN_SIZE 11
#define TRELL_CONSOLE_SIZE VGA_COLS - TRELL_RCOLUMN_SIZE
#define TRELL_CONSOLE_LINES 23

#define TRELL_MAX_HISTORY 1024

//static char __trell_lines[TRELL_MAX_HISTORY][TRELL_CONSOLE_SIZE];
//static char __trell_current_apps[10][TRELL_RCOLUMN_SIZE];

static vga_char_attrib_t menu_color = {
    .bg = VGA_WHITE,
    .font = VGA_BLACK
};

static vga_char_attrib_t console_color = {
    .bg = VGA_BLUE,
    .font = VGA_WHITE
};

static vga_char_attrib_t title_color = {
    .bg = VGA_LIGHT_BLUE,
    .font = VGA_RED
};

static vga_char_attrib_t app_color = {
    .bg = VGA_LIGHT_GRAY,
    .font = VGA_WHITE
};

static vga_char_attrib_t current_app_color = {
    .bg = VGA_YELLOW,
    .font = VGA_BLACK
};

static void trell_update_ui();

void trell_initialize()
{
    trell_update_ui();
}

void trell_clear()
{

}

static void trell_update_ui()
{
    vga_set_position(0,0);

    for(int i = 0; i<VGA_LINES; i++)
    {
        if(i == 0)
        {
            vga_set_color(&title_color);
        }
        else
        {
            vga_set_color(&console_color);
        }
        for(int j = 0; j<TRELL_CONSOLE_SIZE; j++)
        {
            vga_putch(' ');
        }

        vga_set_color(&menu_color);
        for(int j = 0; j<TRELL_RCOLUMN_SIZE; j++)
        {
            vga_putch(' ');
        }
        vga_putch('\n');
    }

    vga_set_color(&title_color);
    vga_set_position(0,0);
    vga_puts("TrOS-2\0");

    vga_set_position(28,0);
    vga_puts("   Console   \0");

    vga_set_position(TRELL_CONSOLE_SIZE, 0);
    vga_set_color(&current_app_color);
    vga_puts("01: Conso..");

    vga_set_color(&app_color);
    for(int i = 1; i<2; i++)
    {
        vga_set_position(TRELL_CONSOLE_SIZE, i);
        vga_puts("04: App    ");
        //printk("0%d: App %d  ", i, i);
    }
}

void trell_putch(char c)
{

}
void trell_puts(const char* str)
{

}
