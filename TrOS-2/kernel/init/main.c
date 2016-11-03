// Kernel.c
// TrOS kernel - Main entry

#include <tros/tros.h>
#include <tros/timer.h>
#include <tros/irq.h>
#include <tros/pmm.h>	// REMOVE
#include <tros/vmm.h> 	// REMOVE
#include <tros/fs/vfs.h>
#include <tros/kheap.h>
#include <tros/exec.h>
#include <tros/hwdetect.h>
#include <tros/sys/multiboot.h>

#include <tros/process.h>

#include <tros/klib/kstring.h>

#define DEFN_SYSCALL1(fn, num, P1) \
int syscall_##fn(P1 p1) \
{ \
    int a; \
    __asm("int $0x80" : "=a" (a) : "0" (num), "b" ((int)p1)); \
    return a; \
}

#define DEFN_SYSCALL3(fn, num, P1, P2, P3) \
int syscall_##fn(P1 p1, P2 p2, P3 p3) \
{ \
    int a; \
    __asm("int $0x80" : "=a" (a) : "0" (num), "b" ((int)p1), "c" ((int)p2), "d"((int)p3)); \
    return a; \
}

DEFN_SYSCALL1(open, 5, char*);
DEFN_SYSCALL3(write, 8, unsigned int, const void*, unsigned int);


//Drivers baked in to the kernel
extern int kbd_driver_initialize();
extern int floppy_driver_initialize(unsigned char device);
extern int vga_driver_initialize();

//Filesystems baked in to the kernel
extern int fat12_fs_initialize();

//other, maybe use a correct header file?
extern void syscall_initialize();
extern void serial_init();

void kernel_idle();
void kernel_ring3_test();

void kernel_early()
{
    irq_initialize();
    serial_init();
    timer_initialize(50);

    __asm("sti");
}

void kernel_memory_initialize(uint32_t stack_top, multiboot_info_t* multiboot)
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
    kernel_memory_initialize(stack_top, multiboot);
    kernel_drivers();
    kernel_filesystems();

    if(!vfs_mount("fdd", "fat12"))
    {
        kernel_panic("Error mounting root folder. Halting!", 0);
    }

    syscall_initialize();
    printk("Creating kernel idle proc and starting shell\n\n");

    // Make a Kernel Idle process with the current page-dir.
    process_create_idle(&kernel_idle, vmm_get_directory());
    //process_exec_user(&kernel_ring3_test);

    char* argv[] =
    {
        "/bin/trell"
    };
    exec_elf32(argv[0], 1, argv);

    while(1)
    {
        kernel_panic("Reeached END OF KERNEL", 0);
    }
}

// This is the "idle process"
void kernel_idle()
{
    //This test code runs in ring1 - Kernelspace
    while(1)
    {
        __asm("sti");
        __asm("hlt;");
    }
}

void kernel_ring3_test()
{
    //This test code runs in ring3 - Userland
    unsigned int vga = syscall_open("vga");
    int character = (int)'u';
    while(1)
    {
        //BOCHS_DEBUG;
        syscall_write(vga, &character, 1);
    }
}
