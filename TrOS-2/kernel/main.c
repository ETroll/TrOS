// Kernel.c
// TrOS kernel - Main entry

#include <TrOS/TrOS.h>
#include <TrOS/hal/idt.h>
#include <TrOS/hal/gdt.h>
#include <TrOS/hal/VGA.h>

void kernel_early()
{
    //"init" VGA
    vga_char_attrib_t clr = {
    	.bg = VGA_BLACK,
        .font = VGA_WHITE
    };
    vga_clear_screen(&clr);
    vga_set_color(&clr);

    gdt_initialize();
    idt_initialize();
}

void kernel_main()
{
    kernel_early();

    GenerateInterrupt(0x15);
    GenerateInterrupt(0x26);
    printk("This is the end!\n");
    //dummy end stuff
    // vga_set_position(0,VGA_LINES-1);
    // printk("> ");
    // vga_move_cursor(2,VGA_LINES-1);
    //__asm("sti");
    __asm("hlt");
    while(1)
    {
        __asm("nop;");
    }
}
