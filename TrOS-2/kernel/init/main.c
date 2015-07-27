// Kernel.c
// TrOS kernel - Main entry

#include <tros/tros.h>
#include <tros/hal/VGA.h>
#include <tros/scheduler.h>
#include <tros/irq.h>
#include <tros/driver.h>
#include <tros/pmm.h>
#include <tros/vmm.h>
#include <multiboot.h>



extern int kbd_driver_initialize();

void kernel_early()
{
	//"init" VGA
	vga_char_attrib_t clr = {
		.bg = VGA_BLACK,
		.font = VGA_WHITE
	};
	vga_clear_screen(&clr);
	vga_set_color(&clr);

	//IRQ and Scheduling
	irq_initialize();
	scheduler_initialize(50);

	__asm("sti");
}

void kernel_drivers()
{
	driver_initialize();
	kbd_driver_initialize();
}

void kernel_main(multiboot_info_t* multiboot, uint32_t kernel_size, uint32_t magic)
{
    kernel_early();

    uint32_t memSize = 1024 + multiboot->memoryLo + multiboot->memoryHi*64;
    pmm_region_t* regions = (pmm_region_t*)0x1000;

    pmm_initialize(0x100000 + (kernel_size*512), memSize, regions);
	pmm_deinit_region(0x0, 0x100000); //Dont want to use first mb
	pmm_deinit_region(0x100000, kernel_size*512);

	printk("\nBlocks initialized: %i\nUsed or reserved blocks: %i\nFree blocks: %i\n\n",
		pmm_get_block_count(),
		pmm_get_use_block_count(),
		pmm_get_free_block_count());
	vmm_initialize();

	kernel_drivers();

    while(1)
    {
        __asm("nop;");
    }
}
