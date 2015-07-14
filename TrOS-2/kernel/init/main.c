// Kernel.c
// TrOS kernel - Main entry

#include <tros/TrOS.h>
#include <tros/hal/VGA.h>
#include <tros/scheduler.h>
#include <tros/irq.h>
#include <multiboot.h>

void kernel_early()
{
    //"init" VGA
    vga_char_attrib_t clr = {
    	.bg = VGA_BLACK,
        .font = VGA_WHITE
    };
    vga_clear_screen(&clr);
    vga_set_color(&clr);



    irq_initialize();
    scheduler_initialize(50);

}

void kernel_main(multiboot_info_t* multiboot, uint32_t kernel_size, uint32_t magic)
{
    kernel_early();

    uint32_t memSize = 1024 + multiboot->memoryLo + multiboot->memoryHi*64;

    printk("Got multiboot at: %x\n", multiboot);
    printk("Kernel size: %d KB\n", (kernel_size*512)/1024); //it is in sectors
    printk("Memory size %d MB\n", memSize/1024);

    //GenerateInterrupt(0x15);
    //GenerateInterrupt(0x26);
    printk("Lets enable IRQ\n");

    __asm("sti");   //Bad!

    //dummy end stuff
    // vga_set_position(0,VGA_LINES-1);
    // printk("> ");
    // vga_move_cursor(2,VGA_LINES-1);
    //__asm("sti");
    //__asm("hlt");
    while(1)
    {
        __asm("nop;");
    }
}
