#include <trlib/threading.h>
#include <trlib/syscall.h>
#include <stdint.h>

thread_t thread_start(void (*func)())
{
    return syscall_threadstart((uint32_t)func, (uint32_t)thread_exit);
}

void thread_cancel(thread_t thread)
{
    syscall_threadcancel(thread);
}

void thread_exit()
{
    syscall_threadexit();
}

void thread_sleep(uint32_t ms)
{
    syscall_threadsleep(ms);
}
