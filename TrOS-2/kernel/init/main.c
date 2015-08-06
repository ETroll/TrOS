// Kernel.c
// TrOS kernel - Main entry

#include <tros/tros.h>
#include <tros/hal/VGA.h>
#include <tros/scheduler.h>
#include <tros/irq.h>
#include <tros/driver.h>
#include <tros/pmm.h>
#include <tros/vmm.h>
#include <tros/hwdetect.h>
#include <multiboot.h>

#include <trell/trell.h>

extern int kbd_driver_initialize();
extern int floppy_driver_initialize(unsigned char device);
extern int vga_driver_initialize();

void (*__putch)(char c);
void (*__puts)(const char* str);

void kernel_tmp_cmd();

void kernel_early()
{
	//"init" VGA
	vga_char_attrib_t clr = {
		.bg = VGA_BLACK,
		.font = VGA_WHITE
	};
	vga_clear_screen(&clr);
	vga_set_color(&clr);

	__putch = vga_putch;
	__puts = vga_puts;

	//IRQ and Scheduling
	irq_initialize();
	scheduler_initialize(50);

	__asm("sti");
}

void kernel_drivers()
{
	driver_initialize();

	hwd_floppy_t fd = hwdetect_floppy_disks();
	printk("FDC Primary is:   %s\n", fd.master_desc);
	printk("FDC Secondary is: %s\n\n", fd.slave_desc);

	kbd_driver_initialize();
	if(fd.master > 0)
	{
		floppy_driver_initialize(0);
	}
	if(fd.slave > 0)
	{
		floppy_driver_initialize(1);
	}

	vga_driver_initialize();

	//TODO: RAMDISK DRIVER
	//TODO: VFS DRIVER
	//TODO: FAT DRIVER?
}

void kernel_main(multiboot_info_t* multiboot, uint32_t kernel_size, uint32_t magic)
{
    kernel_early();

    uint32_t memSize = 1024 + multiboot->memoryLo + multiboot->memoryHi*64;
    pmm_region_t* regions = (pmm_region_t*)0x1000;

    pmm_initialize(0xC0000000 + (kernel_size*512), memSize, regions);

	//pmm_deinit_region(0x0, 0x100000); //Dont want to use first mb
	pmm_deinit_region(0x100000, kernel_size*512);

	printk("\nBlocks initialized: %i\nUsed or reserved blocks: %i\nFree blocks: %i\n\n",
		pmm_get_block_count(),
		pmm_get_use_block_count(),
		pmm_get_free_block_count());

	vmm_initialize();

	kernel_drivers();


	//Lets set up basic console
	trell_initialize();

    while(1)
    {
        __asm("nop;");
    }
}
