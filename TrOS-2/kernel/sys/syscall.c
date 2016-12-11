#include <tros/irq.h>
#include <tros/mem/vmm2.h>
#include <tros/driver.h>
#include <tros/tros.h>
#include <tros/process.h>

//NOTE: Maybe move each syscall into own file in a folder?

#define MAX_SYSCALL 25
static void* _syscalls[MAX_SYSCALL];
//static void syscall_dispatcher(cpu_registers_t *regs);

static int sys_debug(unsigned int method)
{
    //TODO: Syscall for basic debug functionality
    //      different parameters selects different dumps from the kernel
    return -1;
}

static int sys_getpid()
{
    process_t* process = process_get_current();
    return process;
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
    // BOCHS_DEBUG;
    //printk("Trying to open %s - %x\n", name, name);
    device_driver_t* device = driver_find_device(name);
    if(device != 0)
    {
        int result = -1;
        switch (device->type) {
            case DRV_BLOCK:
                result = ((driver_block_t*)device->driver)->open();
            break;
            case DRV_HID:
                result = ((driver_hid_t*)device->driver)->open();
            break;
            case DRV_CHAR:
                result = ((driver_char_t*)device->driver)->open();
            break;
        }
        if(result > 0)
        {
            //printk("Found: %i\n", device->id);
            return device->id;
        }
        else
        {
            //printk("Error: Did not find device\n");
            return result;
        }
    }
    return -1;
}

static int sys_close(unsigned int fd)
{
    device_driver_t* device = driver_find_device_id(fd);
    if(device != 0)
    {
        switch (device->type) {
            case DRV_BLOCK:
                ((driver_block_t*)device->driver)->close();
            break;
            case DRV_HID:
                ((driver_hid_t*)device->driver)->close();
            break;
            case DRV_CHAR:
                ((driver_char_t*)device->driver)->close();
            break;
        }
        return 1;
    }
    return -1;
}

static int sys_seek(unsigned int fd, unsigned int pos)
{
    device_driver_t* device = driver_find_device_id(fd);
    if(device != 0)
    {
        if(device->type == DRV_CHAR)
        {
            return ((driver_char_t*)device->driver)->seek(pos);
        }
    }
    return -1;
}

static int sys_write(unsigned int fd, const void *buffer, unsigned int count)
{
    device_driver_t* device = driver_find_device_id(fd);
    if(device != 0)
    {
        if(device->type == DRV_CHAR)
        {
            return ((driver_char_t*)device->driver)->write((char*)buffer, count);
        }
    }
    return -1;
}

static int sys_read(unsigned int fd, void *buffer, unsigned int count)
{
    device_driver_t* device = driver_find_device_id(fd);
    if(device != 0)
    {
        if(device->type == DRV_CHAR)
        {
            return ((driver_char_t*)device->driver)->read((char*)buffer, count);
        }
    }
    return -1;
}

static int sys_ioctl(unsigned int fd, unsigned int ioctl_num, unsigned int param)
{
    device_driver_t* device = driver_find_device_id(fd);
    if(device != 0)
    {
        if(device->type == DRV_CHAR)
        {
            return ((driver_char_t*)device->driver)->ioctl(ioctl_num, param);
        }
    }
    return -1;
}

//TODO: Fix or redo the "hid" driver
static int sys_read_hid(unsigned int fd, void *buffer, unsigned int count)
{
    device_driver_t* device = driver_find_device_id(fd);
    if(device != 0)
    {
        if(device->type == DRV_HID)
        {
            return ((driver_hid_t*)device->driver)->read((int*)buffer, count);
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

int syscall_dispatcher(syscall_parameters_t regs)
{
    //printk("Dispatching syscall: %i\n", regs->eax);
    //printk("jall\n");
    //int test = 2;
    // BOCHS_DEBUG;
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
    _syscalls[1] = &sys_exit;
    _syscalls[2] = &sys_sleep;
    _syscalls[3] = &sys_open;
    _syscalls[4] = &sys_close;
    _syscalls[5] = &sys_seek;
    _syscalls[6] = &sys_write;
    _syscalls[7] = &sys_read;
    _syscalls[8] = &sys_ioctl;
    _syscalls[9] = &sys_increasemem;
    _syscalls[10] = &sys_decreasemem;
    _syscalls[11] = &sys_debug;
    _syscalls[12] = &sys_read_hid;

    //irq_register_handler(0x80, &syscall_dispatcher);
}
