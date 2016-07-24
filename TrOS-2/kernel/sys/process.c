#include <tros/process.h>
#include <tros/kheap.h>
#include <tros/tros.h>
#include <tros/hal/tss.h>
#include <tros/vmm.h>

#define PROC_STACK_SIZE 16384 //16K stack?

static process_t* _current_process = 0;

static process_t* _processes[10]; //just 10 while testing..
static unsigned int num_proc = 0;

extern void enter_usermode(unsigned int location, unsigned int userstack);

void process_preempt()
{
    if(_current_process != 0)
    {
        process_switchto(_current_process->next);
    }
}

// void process_create(process_t *task, void (*main)(), unsigned int flags, unsigned int* pagedir)
// {
//     printk("Creating process - Main: %x Flags: %x CR3: %x\n", main, flags, pagedir);
//     task->regs.eax = 0;
//     task->regs.ebx = 0;
//     task->regs.ecx = 0;
//     task->regs.edx = 0;
//     task->regs.esi = 0;
//     task->regs.edi = 0;
//     task->regs.eflags = flags;
//     task->regs.eip = (unsigned int)main;
//     task->regs.cr3 = (unsigned int)pagedir;
//     task->regs.esp = (unsigned int)kmalloc(0x1000) + 0xFFC; // Not implemented here
//     task->next = 0;
// }

void process_switchto(process_t* next)
{
    printk("SWCH TO EIP: %x CR3: %x ESP: %x kESP: %x pid: %d\n",
        next->regs.eip,
        next->regs.cr3,
        next->regs.esp,
        next->thread.kernel_stack_ptr,
        next->pid);

    tss_set_ring0_stack(0x10, next->thread.kernel_stack_ptr);

    _current_process = next;
    process_t *prev = _current_process;
    process_switch(&prev->regs, &_current_process->regs);
}

void process_exec_user(void (*main)())
{
    //Only run ONCE!
    process_t* proc = (process_t*)kmalloc(sizeof(process_t));
    proc->pagedir = vmm_clone_directory(vmm_get_directory());
    proc->pid = num_proc;

    if(num_proc > 0)
    {
        process_t* prev = _processes[num_proc-1];
        //printk("Next proc %x\n", _processes[0]);
        proc->next = _processes[0];
        prev->next = proc;
    }
    else
    {
        printk("Looping....\n");
        proc->next = proc; //loop
    }

    proc->thread.user_stack_ptr = (unsigned int)kmalloc(PROC_STACK_SIZE) + (PROC_STACK_SIZE-sizeof(unsigned int));
    proc->thread.kernel_stack_ptr = (unsigned int)kmalloc(PROC_STACK_SIZE) + (PROC_STACK_SIZE-sizeof(unsigned int));
    proc->thread.instr_ptr = (unsigned int)main;
    proc->thread.priority = 1;
    proc->thread.state = 0;

    proc->regs.eax = 0;
    proc->regs.ebx = 0;
    proc->regs.ecx = 0;
    proc->regs.edx = 0;
    proc->regs.esi = 0;
    proc->regs.edi = 0;
    proc->regs.eip = proc->thread.instr_ptr;
    proc->regs.cr3 = (unsigned int)proc->pagedir->entries;
    proc->regs.esp = proc->thread.user_stack_ptr;
    proc->regs.eflags = _current_process->regs.eflags;

    _processes[num_proc++] = proc;
    _current_process = proc;

    printk("USER proc %x - EIP %x ESP: %x kESP: %x\n",
        proc,
        proc->regs.eip,
        proc->regs.esp,
        proc->thread.kernel_stack_ptr);

    tss_set_ring0_stack(0x10, proc->thread.kernel_stack_ptr);
    enter_usermode(proc->thread.instr_ptr, proc->thread.user_stack_ptr);
}

void process_create_idle(void (*main)())
{
    //Only to be run ONCE!
    if(_current_process == 0)
    {
        process_t* idleproc = (process_t*)kmalloc(sizeof(process_t));
        idleproc->pagedir = vmm_get_directory();
        idleproc->next = idleproc; //loop
        idleproc->pid = num_proc;

        idleproc->thread.user_stack_ptr = 0;
        idleproc->thread.kernel_stack_ptr = (unsigned int)kmalloc(PROC_STACK_SIZE) + (PROC_STACK_SIZE-sizeof(unsigned int));
        idleproc->thread.instr_ptr = (unsigned int)main;
        idleproc->thread.priority = -1;
        idleproc->thread.state = 0;

        idleproc->regs.eax = 0;
        idleproc->regs.ebx = 0;
        idleproc->regs.ecx = 0;
        idleproc->regs.edx = 0;
        idleproc->regs.esi = 0;
        idleproc->regs.edi = 0;
        idleproc->regs.eip = idleproc->thread.instr_ptr;
        idleproc->regs.cr3 = (unsigned int)idleproc->pagedir->entries;
        idleproc->regs.esp = idleproc->thread.kernel_stack_ptr;
        __asm("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(idleproc->regs.eflags)::"%eax");

        _processes[num_proc++] = idleproc;
        _current_process = idleproc;

        printk("IDLE proc %x - EIP %x ESP: %x kESP: %x\n",
            idleproc,
            idleproc->regs.eip,
            idleproc->regs.esp,
            idleproc->thread.kernel_stack_ptr);
    }
    else
    {
        //PANIC!
    }
}
