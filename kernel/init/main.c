// Kernel.c
// TrOS kernel - Main entry

#include <tros/tros.h>
#include <tros/timer.h>
#include <tros/irq.h>
#include <tros/fs/vfs.h>
#include <tros/memory.h>
#include <tros/hwdetect.h>
#include <tros/sys/multiboot.h>
#include <tros/sched/scheduler.h>

#include <tros/klib/kstring.h>

//Drivers baked in to the kernel
extern int kbd_driver_initialize();
extern int floppy_driver_initialize(unsigned char device);
extern int textmode_driver_initialize();

//Filesystems baked in to the kernel
extern int fat12_fs_initialize();

//other, maybe use a correct header file?
extern void syscall_initialize();
extern void serial_init();

int kernel_idle();

// #define UNDERDEV 1

void kernel_early()
{
    irq_initialize();
    serial_init();
    timer_initialize(1000);

    __asm("sti");
}

void kernel_memory_initialize(uint32_t stack_top, multiboot_info_t* multiboot)
{
    uint32_t memSize = 1024 + multiboot->memoryLo + multiboot->memoryHi*64;
    mem_usage_physical_t usage;

    memory_initialize(stack_top, memSize, 0x1000);
    memory_physical_usage(&usage);
    printk("Kernel stack top at: %x \n", stack_top);
    printk("\nBlocks initialized: %d\nUsed or reserved blocks: %d\nFree blocks: %d\n\n",
        usage.total,
        usage.used,
        usage.free);
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
    textmode_driver_initialize() ? printk("OK\n") : printk("FAILED!\n");
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
    /*
        fd0/
            initrd
            kernel.elf
            krnldr.bin
            test
            folder/
                test1
                test2
                ...
        hd0/
        cd0/
        dvd0/
    */
    syscall_initialize();
    scheduler_initialize();
    scheduler_executeKernel(&kernel_idle);

    while(1)
    {
        kernel_panic("Reached END OF KERNEL", 0);
    }
}

// This is the "init/idle process"
int kernel_idle()
{
    printk("Starting INIT process!\n");
    if(!vfs_mount("fd0", "fat12"))
    {
        kernel_panic("Error mounting root folder. Halting!", 0);
    }

    char* argv[] =
    {
        "/fd0/trell"
    };
    process_t* proc = scheduler_executeUser(1, argv);
    printk("Started Trell at PID: %d starting idle loop\n", proc->pid);

    while(1)
    {
        __asm("sti");
        __asm("hlt;");
    }
    return 0;
}
