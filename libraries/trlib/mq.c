#include <trlib/mq.h>
#include <trlib/syscall.h>

int mq_send(uint32_t dest, void* data, uint32_t size, mq_flags_t flags)
{
    return syscall_sendmessage(dest, data, size, flags);
}

int mq_recv(void* buffer, uint32_t size, mq_flags_t flags)
{
    return syscall_readmessage(buffer, size, flags);
}
