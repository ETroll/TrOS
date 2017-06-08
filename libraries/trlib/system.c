#include <trlib/system.h>
#include <trlib/syscall.h>

int system_debug(debug_flags_t flag)
{
    return syscall_debug(flag);
}

int system_pid()
{
    return syscall_getpid();
}

int system_parentpid()
{
    return syscall_getparentpid();
}

int system_execute(char** params)
{
    return syscall_execute(params);
}

int system_kill(uint32_t pid)
{
    return -1;
}

void system_exit(int code)
{
    syscall_exit(code);
}
