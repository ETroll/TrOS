#include <tros/irq.h>
#include <tros/mem/vmm2.h>
#include <tros/driver.h>
#include <tros/tros.h>
#include <tros/klib/kstring.h>
#include <tros/sched/scheduler.h>
#include <stdint.h>

//NOTE: Maybe move each syscall into own file in a folder?

#define MAX_SYSCALL 25
static void* _syscalls[MAX_SYSCALL];

extern uint32_t paging_get_CR3();

static int sys_debug(unsigned int method)
{
    uint32_t pid = scheduler_getCurrentProcess()->pid;
    printk("DEBUG(%d): Data %x\n", pid, method);
    if(method == 0x1)
    {
        return paging_get_CR3();
    }
    return method;
}

static int sys_getpid()
{
    process_t* process = scheduler_getCurrentProcess();
    return process->pid;
}

static int sys_get_parent_pid()
{
    process_t* process = scheduler_getCurrentProcess();
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
    process_t* process = scheduler_getCurrentProcess();
    uint32_t start = process->heapendAddr;
    vmm2_map(start, blocks,  VMM2_PAGE_USER | VMM2_PAGE_WRITABLE);

    process->heapendAddr += (VMM2_BLOCK_SIZE * blocks);

    return start;
}

static int sys_decreasemem(unsigned int blocks)
{
    return -1;
}

static int sys_sendmessage(uint32_t pid, const void* data, uint32_t size, uint32_t flags)
{
    process_t* reciever = scheduler_getProcessFromPid(pid);
    process_t* sender = scheduler_getCurrentProcess();

    if(reciever && sender)
    {
        printk("sys_sendmessage Payload (%d): %s\n", size, data);
        mailbox_message_t* message = mailbox_message_create(sender->pid, data, size, flags);
        if(message)
        {
            mailbox_push(reciever->mailbox, message);
            return size;
        }
    }
    return -1;
}

static int sys_readmessage(void* buffer, uint32_t size, uint32_t flags)
{
    process_t* proc = scheduler_getCurrentProcess();

    mailbox_message_t* message = mailbox_pop(proc->mailbox);
    if(message != 0)
    {
        uint32_t sizetoread = size;
        if(size > message->size)
        {
            sizetoread = message->size;
        }

        printk("sys_readmessage Payload (%d): %s\n",sizetoread, message->payload);
        memcpy(buffer, message->payload, sizetoread);

        mailbox_message_dispose(message);

        return sizetoread; //TODO: replace with real read num
    }
    return -1;
}

static int sys_execute(const char** arguments)
{
    uint32_t pid = scheduler_getCurrentProcess()->pid;

    printk("EXEC(%d): Trying to execute: %s\n", pid, arguments[0]);
    int i = 0;
    for(; arguments[i] != 0 && i<10;i++)
    {
        printk("EXEC(%d): Argument %d - %s\n", pid, i, arguments[i]);
    }
    //NOTE: Check arguments and set some failover? (If no NULL is given)
    process_t* proc = scheduler_executeUser(i, (char**)arguments);
    printk("EXEC(%d): Complete with PID %d\n", pid, proc->pid);
    return proc->pid;
}

static void sys_exit(uint32_t status)
{
    process_t* proc = scheduler_getCurrentProcess();
    printk("EXIT(%d): Exiting with code: %d\n", proc->pid, status);
    scheduler_removeProcess(proc);
    scheduler_reschedule();
}

static int sys_thread_start(void (*func)(), void (*exit)())
{
    return -1;
}

static void sys_thread_cancel(uint32_t tid)
{
    process_t* proc = scheduler_getCurrentProcess();
    printk("THREAD_CANCEL(%d): TID %d\n", proc->pid, tid);
    scheduler_removeThread(scheduler_getThreadFromTid(tid));
    if(scheduler_getCurrentThread()->tid == tid)
    {
        scheduler_reschedule();
    }
}

static void sys_thread_exit()
{
    process_t* proc = scheduler_getCurrentProcess();
    thread_t* thread = scheduler_getCurrentThread();
    printk("THREAD_EXIT(%d): TID %d\n", proc->pid, thread->tid);
    scheduler_removeThread(thread);
    scheduler_reschedule();
}

static void sys_thread_sleep(uint32_t ms)
{

}

int syscall_dispatcher(syscall_parameters_t regs)
{
    int retval = 0;
    if (regs.eax < MAX_SYSCALL)
    {
        void *syscall = _syscalls[regs.eax];
        // printk("SYSCALL %d", regs.eax);
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
    // printk(" retval: %d\n", retval);
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
    _syscalls[6] = &sys_sendmessage;
    _syscalls[7] = &sys_readmessage;
    _syscalls[8] = &sys_get_parent_pid;
    _syscalls[9] = &sys_increasemem;
    _syscalls[10] = &sys_decreasemem;
    _syscalls[11] = &sys_debug;
    _syscalls[12] = &sys_execute;
    _syscalls[13] = &sys_exit;
    _syscalls[14] = &sys_thread_start;
    _syscalls[15] = &sys_thread_cancel;
    _syscalls[16] = &sys_thread_exit;
    _syscalls[17] = &sys_thread_sleep;
}
