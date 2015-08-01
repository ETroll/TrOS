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
#include <string.h>
#include <keyboard.h>

extern int kbd_driver_initialize();
extern int floppy_driver_initialize(unsigned char device);


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

	pmm_deinit_region(0x0, 0x100000); //Dont want to use first mb
	pmm_deinit_region(0x100000, kernel_size*512);

	printk("\nBlocks initialized: %i\nUsed or reserved blocks: %i\nFree blocks: %i\n\n",
		pmm_get_block_count(),
		pmm_get_use_block_count(),
		pmm_get_free_block_count());

	vmm_initialize();

	kernel_drivers();

	kernel_tmp_cmd();

    while(1)
    {
        __asm("nop;");
    }
}

void kernel_run_command(char* cmd)
{
	printk("Command: %s\n", cmd);
	if(strcmp(cmd, "cls") == 0)
	{
		vga_char_attrib_t clr = {
			.bg = VGA_BLACK,
			.font = VGA_WHITE
		};
		vga_clear_screen(&clr);
	}
	else if(strcmp(cmd, "help") == 0)
	{
		printk("TrOS-2 Help:\n");
		printk("Commands:\n");
		printk(" - help: Displays this help message\n");
		printk(" - cls:  Clears the display\n");
	}
	// else if(strcmp(cmd, "readsect") == 0)
	// {
	// 	driver_hid_t* fdd = (driver_hid_t*)driver_find_device("fdd")->driver;
	// 	if(fdd != 0)
	// 	{
	//
	// 	}
	// 	else
	// 	{
	// 		printk("ERROR: Could not find a floppy disk drive\n");
	// 	}
	// }
	else
	{
		printk("Unknown command\n");
	}
}

void kernel_tmp_cmd()
{
	char cmd_buffer[100];
	int buffer_loc = 0;
	int next_command = 0;
	int key;
	driver_hid_t* kbd = (driver_hid_t*)driver_find_device("kbd")->driver;
	kbd->open();
	while(1)
	{
		buffer_loc = 0;
		next_command = 0;

		printk("> ");

		while(!next_command)
		{
			key = 0;
			if(kbd->read(&key,1))
			{
				if(key != KEY_LCTRL
					&& key != KEY_RCTRL
					&& key != KEY_RALT
					&& key != KEY_LALT
					&& key != KEY_RSHIFT
					&& key != KEY_LSHIFT
					&& key != KEY_CAPSLOCK)
				{
					if(key == KEY_RETURN)
					{
						cmd_buffer[buffer_loc] = '\0';
						printk("\n");
						kernel_run_command(cmd_buffer);
						next_command = 1;
					}
					else if(key == KEY_BACKSPACE)
					{
						if(buffer_loc > 0)
						{
							cmd_buffer[buffer_loc-1] = ' ';
							buffer_loc--;

							vga_position_t pos = vga_get_position();
							pos.x--;
							vga_set_position(pos.x, pos.y);
							vga_putch(' ');
							vga_set_position(pos.x, pos.y);
						}
					}
					else
					{
						if(buffer_loc < 100)
						{
							cmd_buffer[buffer_loc++] = key;
							vga_putch((char)key);
						}
					}
				}
			}
		}
	}
	kbd->close();
}
