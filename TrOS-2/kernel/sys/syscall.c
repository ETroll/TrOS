#include <tros/syscall.h>
#include <tros/irq.h>

#define MAX_SYSCALL 10

static void* _syscalls[MAX_SYSCALL];

static void syscall_dispatcher(cpu_registers_t *regs);

void syscall_initialize()
{
    irq_register_handler(0x80, &syscall_dispatcher);
}

void syscall_dispatcher(cpu_registers_t *regs)
{
    if (regs->eax < MAX_SYSCALL)
    {
        void *syscall = _syscalls[regs->eax];
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
