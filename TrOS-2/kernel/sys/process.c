#include <tros/process.h>
#include <tros/kheap.h>

static process_t* current_process;

void process_preempt()
{
    process_t *last = current_process;
    current_process = current_process->next;
    process_switch(&last->regs, &current_process->regs);
}

void process_create(process_t *task, void (*main)(), unsigned int flags, unsigned int* pagedir)
{
    task->regs.eax = 0;
    task->regs.ebx = 0;
    task->regs.ecx = 0;
    task->regs.edx = 0;
    task->regs.esi = 0;
    task->regs.edi = 0;
    task->regs.eflags = flags;
    task->regs.eip = (unsigned int)main;
    task->regs.cr3 = (unsigned int)pagedir;
    task->regs.esp = (unsigned int)kmalloc(0x1000) + 0x1000; // Not implemented here
    task->next = 0;
}
