// Kernel.c
// TrOS kernel - Main entry
#include <TrOS/TrOS.h>
#include <TrOS/hal/VGA.h>
#include <TrOS/hal/idt.h>
#include <TrOS/hal/gdt.h>

extern void fun_stuff(void);
#define GenerateInterrupt(arg) __asm__("int %0\n" : : "N"((arg)) : "cc", "memory")

// Just a dummy "UI" for experimenting
void draw_ui()
{
    vga_char_attrib_t menu_color = {
    	.bg = VGA_WHITE,
        .font = VGA_BLACK
    };

    vga_char_attrib_t console_color = {
    	.bg = VGA_BLUE,
        .font = VGA_WHITE
    };

    vga_char_attrib_t title_color = {
    	.bg = VGA_LIGHT_BLUE,
        .font = VGA_RED
    };

    vga_char_attrib_t app_color = {
    	.bg = VGA_LIGHT_GRAY,
        .font = VGA_WHITE
    };

    vga_char_attrib_t current_app_color = {
    	.bg = VGA_YELLOW,
        .font = VGA_BLACK
    };

    vga_set_position(0,0);
    unsigned int menu_width = 11;
    unsigned int console_width = VGA_COLS-menu_width;

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
        for(int j = 0; j<console_width; j++)
        {
            vga_putch(' ');
        }

        vga_set_color(&menu_color);
        for(int j = 0; j<menu_width; j++)
        {
            vga_putch(' ');
        }
    }

    vga_set_color(&title_color);
    vga_set_position(0,0);
    printk("TrOS-2\0");

    vga_set_position(28,0);
    printk("=[ Console ]=\0");

    vga_set_position(console_width, 0);
    vga_set_color(&current_app_color);
    printk("01: Conso..");

    vga_set_color(&app_color);
    for(int i = 2; i<6; i++)
    {
        vga_set_position(console_width, i-1);
        printk("0%d: App %d  ", i, i);
    }
}

void kernel_early()
{
    vga_char_attrib_t console_color = {
    	.bg = VGA_BLUE,
        .font = VGA_WHITE
    };

    vga_set_color(&console_color);
    vga_set_position(0,1);

    gdt_initialize();
    idt_initialize();
}

void kernel_main()
{
    draw_ui();
    kernel_early();

    GenerateInterrupt(0x15);
    GenerateInterrupt(0x26);

    //dummy end stuff
    vga_set_position(0,VGA_LINES-1);
    printk("> ");
    vga_move_cursor(2,VGA_LINES-1);
    //__asm("sti");
    __asm("hlt");
    while(1)
    {
        __asm("nop;");
    }
}
