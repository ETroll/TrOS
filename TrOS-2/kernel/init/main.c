// Kernel.c
// TrOS kernel - Main entry

#include <tros/tros.h>
#include <tros/timer.h>
#include <tros/irq.h>
#include <tros/pmm.h>	// REMOVE
#include <tros/vmm.h> 	// REMOVE
#include <tros/fs/vfs.h>
#include <tros/kheap.h>
#include <tros/hwdetect.h>
#include <tros/sys/multiboot.h>

#include <tros/process.h>

#include <tros/klib/kstring.h>

//Drivers baked in to the kernel
extern int kbd_driver_initialize();
extern int floppy_driver_initialize(unsigned char device);
extern int vga_driver_initialize();

//Filesystems baked in to the kernel
extern int fat12_fs_initialize();

//other, maybe use a correct header file?
extern void syscall_initialize();

extern void serial_init();

unsigned int tmp_userland_stack[1024];

void kernel_early()
{
    irq_initialize();
    serial_init();
    timer_initialize(50);

    __asm("sti");
}

void kernel_memory(uint32_t stack_top, multiboot_info_t* multiboot)
{
    uint32_t memSize = 1024 + multiboot->memoryLo + multiboot->memoryHi*64;
    pmm_region_t* regions = (pmm_region_t*)0x1000;

    int mmap_size = pmm_initialize(stack_top, memSize, regions);
    printk("Kernel stack top at: %x \n", stack_top);
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
    fat12_fs_initialize();
}

void kernel_main(multiboot_info_t* multiboot, uint32_t magic, uint32_t stack_top)
{
    kernel_early();

    kernel_memory(stack_top, multiboot);
    kernel_drivers();
    kernel_filesystems();

    if(!vfs_mount("fdd", "fat12"))
    {
        printk("Error mounting root folder. Halting!\n");
        __asm("cli;");
        __asm("hlt;");
    }

    syscall_initialize();

    char* path = "/fdd/FOLDER/";
    // Test / DEBUG
    fs_node_t* root = kopen(path);
    printk("Directory %s (%x)\n", path, root);
    unsigned int index = 0;
    dirent_t* dirent = vfs_readdir(root, index);
    printk("Dirent: %x(%d)\n", dirent, index);
    while (dirent != 0)
    {
        printk("%s - ", dirent->name);
        if(dirent->flags & VFS_FLAG_DIRECTORY)
        {
            printk("DIR");
        }
        else
        {
            printk("FILE");
        }
        printk(" %x\n", dirent->name, dirent->flags);
        kfree(dirent);

        dirent = vfs_readdir(root, ++index);
    }
    vfs_close(root);


    fs_node_t* testfile = kopen("/fdd/trell");
    if(testfile != 0)
    {
        printk("File found at %x\n", testfile);

        unsigned int* filebuffer = (unsigned int*)kmalloc(testfile->size+1);
        unsigned int read_bytes = vfs_read(testfile, 0, testfile->size, (unsigned char*)filebuffer);

        if(read_bytes > 0)
        {
            for(int i = 0; i<testfile->size; i+=4)
            {
                printk("%x ", filebuffer[i]);
            }
        }
        else
        {
            printk("Failure reading file trell\n");
        }
        vfs_close(testfile);
    }


    // End Test / DEBUG

    extern void tss_set_ring0_stack(uint16_t, uint32_t);
    extern void enter_usermode();
    int ring0_stack = 0;
    int ring3_stack = (int)(tmp_userland_stack+1024);

    __asm("mov %%esp, %0;" : "=a"(ring0_stack));
    printk("Installing ring0 stack at %x\n", ring0_stack);
    printk("Installing ring3 stack at %x\n", ring3_stack);

    tss_set_ring0_stack(0x10, ring0_stack);
    __asm("mov %0, %%ESP " : : "m"(ring3_stack));

    //BOCHS_DEBUG;
    //NOTE: The kernel stack should be "empty" at this moment. Except for the
    //      parameters passed into kernel_main via the stack.
    //      So we should be able to clear the stack if the stak has "leaked"?

    // enter_usermode();
    // trell_main();

    while(1)
    {
        __asm("hlt;");
    }
}
