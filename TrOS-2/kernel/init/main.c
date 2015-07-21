// Kernel.c
// TrOS kernel - Main entry

#include <tros/tros.h>
#include <tros/hal/VGA.h>
#include <tros/scheduler.h>
#include <tros/irq.h>
#include <tros/pmm.h>
#include <tros/vmm.h>
#include <multiboot.h>
#include <trell/trell.h>

char* pmm_memory_types[] = {
	"Available",
	"Reserved",
	"ACPI Reclaim",
	"ACPI NVS Memory"
};

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
	__asm("sti");
}

void kernel_main(multiboot_info_t* multiboot, uint32_t kernel_size, uint32_t magic)
{
    kernel_early();

    uint32_t memSize = 1024 + multiboot->memoryLo + multiboot->memoryHi*64;
    pmm_region_t* regions = (pmm_region_t*)0x1000;

    //printk("Kernel size: %d KB\n", (kernel_size*512)/1024);
    //printk("Memory size %d MB\n\n", memSize/1024);

    pmm_initialize(0x100000 + (kernel_size*512), memSize);
    printk("PMM initialized with %d KB physical memory\n\n", memSize);

    printk("Physical Memory Map:\n");
    for(int i=0; i<15; ++i)
    {
        if (regions[i].type > 4)
        {
            regions[i].type = 1;
        }
        if(i > 0 && regions[i].startLo == 0)
        {
            break;
        }
        printk("Region %d: Start %x Length %d KB Type: %d (%s)\n",
            i,
            regions[i].startLo,
            regions[i].sizeLo / 1014,
            regions[i].type,
            pmm_memory_types[regions[i].type-1]);

        if (regions[i].type == 1)
        {
            pmm_init_region(regions[i].startLo, regions[i].sizeLo);
        }
	}
	pmm_deinit_region(0x100000, kernel_size*512);

	printk("\nBlocks initialized: %i\nUsed or reserved blocks: %i\nFree blocks: %i\n",
		pmm_get_block_count(),
		pmm_get_use_block_count(),
		pmm_get_free_block_count());


    //GenerateInterrupt(0x15);
    //GenerateInterrupt(0x26);
    //printk("Lets enable IRQ\n");

	vmm_initialize();
	trell_initialize();

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
