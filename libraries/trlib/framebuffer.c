#include <trlib/framebuffer.h>
#include <trlib/syscall.h>

void framebuffer_swapbuffer(unsigned int device, unsigned char* buffer, unsigned int length)
{
    syscall_swapbuffer(device, buffer, length);
}
