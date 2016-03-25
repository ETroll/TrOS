#include <tros/irq.h>
#include <tros/kheap.h>
#include <tros/driver.h>

#define MAX_SYSCALL 25
static void* _syscalls[MAX_SYSCALL];
//static void syscall_dispatcher(cpu_registers_t *regs);

static int sys_fork()
{
    //TODO
    return -1;
}

static int sys_yield()
{
    //TODO
    return -1;
}

static int sys_getpid()
{
    //TODO
    return -1;
}

static int sys_exit(unsigned int code)
{
    //TODO
    return -1;
}

static int sys_sleep(unsigned int ms)
{
    //TODO
    return -1;
}

static int sys_open(char* name)
{
    device_driver_t* device = driver_find_device(name);
    if(device != 0 && device->open != 0)
    {
        int result = device->open();
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

static int sys_close(unsigned int fd)
{
    device_driver_t* device = driver_find_device_id(fd);
    if(device != 0 && device->close != 0)
    {
        device->close();
        return 1;
    }
    return -1;
}

static int sys_peek(unsigned int fd)
{
    //TODO
    return -1;
}

static int sys_write(unsigned int fd, const void *buffer, unsigned int count)
{
    //TODO
    return -1;
}

static int sys_read(unsigned int fd, void *buffer, unsigned int count)
{
    //TODO
    return -1;
}

static int sys_tmp_kmalloc(unsigned int size)
{
    return (int)kmalloc(size);
}

static void syscall_dispatcher(cpu_registers_t *regs)
{
    if (regs->eax < MAX_SYSCALL)
    {
        void *syscall = _syscalls[regs->eax];
        //printk("Syscall\n");
        if(syscall != 0)
        {
            int retval;
            __asm (" \
                push %1; \
                push %2; \
                push %3; \
                push %4; \
                push %5; \
                call *%6; \
                add %%esp, 20;" : "=a" (retval) : "r" (regs->edi),
                "r" (regs->esi),
                "r" (regs->edx),
                "r" (regs->ecx),
                "r" (regs->ebx),
                "r" (syscall));
            regs->eax = retval;
        }
    }
}

void syscall_initialize()
{
    for(int i = 0; i<MAX_SYSCALL; i++)
    {
        _syscalls[i] = 0;
    }

    _syscalls[0] = &sys_fork;
    _syscalls[1] = &sys_yield;
    _syscalls[2] = &sys_getpid;
    _syscalls[3] = &sys_exit;
    _syscalls[4] = &sys_sleep;
    _syscalls[5] = &sys_open;
    _syscalls[6] = &sys_close;
    _syscalls[7] = &sys_peek;
    _syscalls[8] = &sys_write;
    _syscalls[9] = &sys_read;
    _syscalls[10] = &sys_tmp_kmalloc;

    irq_register_handler(0x80, &syscall_dispatcher);
}
