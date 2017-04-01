#include <tros/irq.h>
#include <tros/mem/vmm2.h>
#include <tros/driver.h>
#include <tros/tros.h>
#include <tros/process.h>
#include <tros/exec.h>
#include <stdint.h>

//NOTE: Maybe move each syscall into own file in a folder?

#define MAX_SYSCALL 25
static void* _syscalls[MAX_SYSCALL];
//static void syscall_dispatcher(cpu_registers_t *regs);

static int sys_debug(unsigned int method)
{
    uint32_t pid = process_get_current()->pid;
    printk("DEBUG(%d): Data %x\n", pid, method);
    return method;
}

static int sys_getpid()
{
    process_t* process = process_get_current();
    return process->pid;
}

static int sys_get_parent_pid()
{
    process_t* process = process_get_current();
    if(process->parent)
    {
        return process->parent->pid;
    }
    return 0;
}

static int sys_opendevice(const char* name)
{
    device_driver_t* device = driver_find_device(name);
    if(device != 0)
    {
        int result = -1;
        switch (device->type)
        {
            case DRV_BLOCK:
                result = ((driver_block_t*)device->driver)->open();
            break;
            case DRV_GENERIC:
                result = ((driver_generic_t*)device->driver)->open();
            break;
        }
        if(result > 0)
        {
            return device->id;
        }
        else
        {
            return result;
        }
    }
    return -1;
}

static int sys_closedevice(unsigned int fd)
{
    device_driver_t* device = driver_find_device_id(fd);
    if(device != 0)
    {
        switch (device->type)
        {
            case DRV_BLOCK:
                ((driver_block_t*)device->driver)->close();
            break;
            case DRV_GENERIC:
                ((driver_generic_t*)device->driver)->close();
            break;
        }
        return 1;
    }
    return -1;
}

static int sys_writedevice(unsigned int fd, const void* buffer, unsigned int count)
{
    device_driver_t* device = driver_find_device_id(fd);
    if(device != 0)
    {
        if(device->type == DRV_GENERIC)
        {
            return ((driver_generic_t*)device->driver)->write((int*)buffer, count);
        }
    }
    return -1;
}

static int sys_readdevice(unsigned int fd, void* buffer, unsigned int count)
{
    device_driver_t* device = driver_find_device_id(fd);
    int read = -1;
    if(device != 0)
    {
        if(device->type == DRV_GENERIC)
        {
            read = ((driver_generic_t*)device->driver)->read((int*)buffer, count);
        }
    }
    return read;
}

static int sys_ioctl(unsigned int fd, unsigned int ioctl_num, unsigned int param)
{
    device_driver_t* device = driver_find_device_id(fd);
    if(device != 0)
    {
        if(device->type == DRV_GENERIC)
        {
            return ((driver_generic_t*)device->driver)->ioctl(ioctl_num, param);
        }
    }
    return -1;
}

static int sys_increasemem(unsigned int blocks)
{
    //returns the start address of the new chunk
    process_t* process = process_get_current();
    uint32_t start = process->heapend_addr;
    vmm2_map(start, blocks,  VMM2_PAGE_USER | VMM2_PAGE_WRITABLE);

    process->heapend_addr += (VMM2_BLOCK_SIZE * blocks);

    return start;
}

static int sys_decreasemem(unsigned int blocks)
{
    return -1;
}

static int sys_sendmessage(uint32_t pid, const void* data, uint32_t size, uint32_t flags)
{
    process_t* reciever = process_get_pid(pid);
    process_t* sender = process_get_current();

    if(reciever && sender)
    {
        mailbox_message_t* message = mailbox_message_create(sender->pid, data, size, flags);
        if(message)
        {
            mailbox_push(reciever->mailbox, message);
            return size;
        }
    }
    return -1;
}

static int sys_readmessage(const void* data, uint32_t size)
{
    //WIP sys_readmessage
    return -1;
}

static int sys_execute(const char* file, const char** argv)
{
    uint32_t pid = process_get_current()->pid;

    printk("EXEC(%d): Trying to execute: %s\n", pid, file);
    int i = 0;
    for(; argv[i] != 0 && i<10;i++)
    {
        printk("EXEC(%d): Argument %d - %s\n", pid, i, argv[i]);
    }

    int retval = exec_file((char*)file, i, (char**)argv);
    printk("EXEC(%d): Complete with PID %d\n", pid, retval);
    return retval;
}

static void sys_exit(uint32_t status)
{
    //WIP: Exit and clean up the process
    // - Clean up all memory used.
    // - Remove from scheduler
    uint32_t pid = process_get_current()->pid;
    printk("EXIT(%d): Exiting with code: %d\n", pid, status);
    process_dispose(process_get_current());
}

int syscall_dispatcher(syscall_parameters_t regs)
{
    int retval = 0;
    if (regs.eax < MAX_SYSCALL)
    {
        void *syscall = _syscalls[regs.eax];
        if(syscall != 0)
        {
            // BOCHS_DEBUG;
            __asm (" \
                push %1; \
                push %2; \
                push %3; \
                push %4; \
                push %5; \
                call *%6; \
                pop %%ebx; \
                pop %%ebx; \
                pop %%ebx; \
                pop %%ebx; \
                pop %%ebx;" : "=a" (retval) : "r" (regs.edi),
                "r" (regs.esi),
                "r" (regs.edx),
                "r" (regs.ecx),
                "r" (regs.ebx),
                "r" (syscall));
            // regs->eax = retval;
        }
    }
    // printk("Syscal retval: %d\n", retval);
    // BOCHS_DEBUG;
    return retval;
}

void syscall_initialize()
{
    for(int i = 0; i<MAX_SYSCALL; i++)
    {
        _syscalls[i] = 0;
    }

    _syscalls[0] = &sys_getpid;
    _syscalls[1] = &sys_opendevice;
    _syscalls[2] = &sys_closedevice;
    _syscalls[3] = &sys_writedevice;
    _syscalls[4] = &sys_readdevice;
    _syscalls[5] = &sys_ioctl;
    _syscalls[6] = &sys_sendmessage;        //new / unmapped
    _syscalls[7] = &sys_readmessage;        //new / unmapped
    _syscalls[8] = &sys_get_parent_pid;
    _syscalls[9] = &sys_increasemem;
    _syscalls[10] = &sys_decreasemem;
    _syscalls[11] = &sys_debug;
    _syscalls[12] = &sys_execute;
    _syscalls[13] = &sys_exit;
}
