#include <trlib/device.h>
#include <trlib/syscall.h>

device_t device_open(char* name)
{
    return syscall_opendevice(name);
}

int device_close(device_t device)
{
    return syscall_closedevice(device);
}

int device_writedata(device_t device, void* data, uint32_t size)
{
    return syscall_writedevice(device, data, size);
}

int device_readdata(device_t device, void* buffer, uint32_t size)
{
    return syscall_readdevice(device, buffer, size);
}

int device_command(device_t device, uint32_t command, uint32_t param)
{
    return syscall_ioctl(device, command, param);
}
