// Kernel.c
// TrOS kernel - Main entry

#include <tros/tros.h>
#include <tros/hal/VGA.h>
#include <tros/scheduler.h>
#include <tros/irq.h>
#include <tros/pmm.h>	// REMOVE
#include <tros/vmm.h> 	// REMOVE
#include <tros/fs/vfs.h>
#include <tros/kheap.h>
#include <tros/hwdetect.h>
#include <multiboot.h>

#include <trell/trell.h>

//Drivers baked in to the kernel
extern int kbd_driver_initialize();
extern int floppy_driver_initialize(unsigned char device);
extern int vga_driver_initialize();

//Filesystems baked in to the kernel
extern int fat16_fs_initialize();

void (*__putch)(char c);
void (*__puts)(const char* str);

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

void kernel_memory(uint32_t stack_top, multiboot_info_t* multiboot)
{
	uint32_t memSize = 1024 + multiboot->memoryLo + multiboot->memoryHi*64;
    pmm_region_t* regions = (pmm_region_t*)0x1000;

    int mmap_size = pmm_initialize(stack_top, memSize, regions);

	unsigned int kernel_region_size = (stack_top-0xC0000000) + mmap_size;
	pmm_deinit_region(0x100000, kernel_region_size);

	printk("\nBlocks initialized: %i\nUsed or reserved blocks: %i\nFree blocks: %i\n\n",
		pmm_get_block_count(),
		pmm_get_use_block_count(),
		pmm_get_free_block_count());

	vmm_initialize();
	kheap_initialize();
}

void kernel_drivers()
{
	hwd_floppy_t fd = hwdetect_floppy_disks();
	if(fd.master > 0)
	{
		floppy_driver_initialize(0) ? printk("OK\n") : printk("FAILED!\n");
	}
	if(fd.slave > 0)
	{
		floppy_driver_initialize(1) ? printk("OK\n") : printk("FAILED!\n");
	}

	kbd_driver_initialize() ? printk("OK\n") : printk("FAILED!\n");
	vga_driver_initialize() ? printk("OK\n") : printk("FAILED!\n");
}

void kernel_filesystems()
{
	vfs_initialize();
	fat16_fs_initialize();
}

void kernel_main(multiboot_info_t* multiboot, uint32_t magic, uint32_t stack_top)
{
    kernel_early();

	kernel_memory(stack_top, multiboot);
	kernel_drivers();
	kernel_filesystems();

	if(!vfs_mount("fdd", "fat16", "/"))
	{
		printk("Error mounting root folder. Halting!\n");
		__asm("cli;");
		__asm("hlt;");
	}

	//Lets set up basic console
	trell_initialize();

    while(1)
    {
        __asm("nop;");
    }
}
