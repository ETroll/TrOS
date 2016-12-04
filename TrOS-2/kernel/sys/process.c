#include <tros/process.h>
#include <tros/memory.h>
#include <tros/tros.h>
#include <tros/hal/tss.h>


#define PROC_STACK_SIZE 16384 //16K stack?

static process_t* _current_process = 0;

static process_t* _processes[10]; //just 10 while testing..
static unsigned int num_proc = 0;

extern void enter_usermode(unsigned int location, unsigned int userstack);

void process_preempt()
{
    if(_current_process != 0 && num_proc > 1)
    {
        // printk("\nprocess_preempt - Current proc: %x CR3: %x\n", _current_process, _current_process->regs.cr3);
        // for(int i = 0; i < num_proc; i++)
        // {
        //     printk("%x process[%d]->next: %x\n",_processes[i], i, _processes[i]->next);
        // }
        process_switchto(_current_process->next);
    }
}

void process_switchto(process_t* next)
{
    // printk("Swtiching to process: %x\n\n", next);
    // printk("SWCH TO EIP: %x CR3: %x ESP: %x kESP: %x pid: %d\n",
    //     next->regs.eip,
    //     next->regs.cr3,
    //     next->regs.esp,
    //     next->thread.kernel_stack_ptr,
    //     next->pid);
    // printk("     EFLAGS: %x\n",
    //     next->regs.eflags);

    //NOTE: Hm.. not good? Do we need to keep this "fresh"?
    tss_set_ring0_stack(0x10, next->thread.kernel_stack_ptr);

    process_t *prev = _current_process;
    _current_process = next;

    process_switch(&prev->regs, &_current_process->regs);
    //printk("DONE!\n");
}

//void process_exec_user(void (*main)())
void process_exec_user(uint32_t startAddr, uint32_t ustack, uint32_t kstack, page_directory_t* pdir)
{
    process_t* proc = (process_t*)kmalloc(sizeof(process_t));
    proc->pagedir = pdir;
    proc->pid = num_proc;

    if(num_proc > 0)
    {
        process_t* prev = _processes[num_proc-1];
        // printk("Next proc %x\n", _processes[0]);
        // printk("Prev proc %x\n", prev);
        proc->next = _processes[0];
        prev->next = proc;


        // printk("Test %x\n", _processes[0]->next);
    }
    else
    {
        // printk("Looping....\n");
        proc->next = proc; //loop
    }

    proc->thread.user_stack_ptr = ustack;
    proc->thread.kernel_stack_ptr = kstack;

    proc->thread.instr_ptr = startAddr;
    proc->thread.priority = 1;
    proc->thread.state = 0;

    proc->regs.eax = 0;
    proc->regs.ebx = 0;
    proc->regs.ecx = 0;
    proc->regs.edx = 0;
    proc->regs.esi = 0;
    proc->regs.edi = 0;
    proc->regs.eip = proc->thread.instr_ptr;
    proc->regs.cr3 = (unsigned int)proc->pagedir->tables;
    proc->regs.esp = proc->thread.user_stack_ptr;
    proc->regs.eflags = _current_process->regs.eflags;

    _processes[num_proc++] = proc;
    _current_process = proc;

    // printk("\nUSER proc %x - EIP %x ESP: %x kESP: %x CR3: %x\n\n",
    //     proc,
    //     proc->regs.eip,
    //     proc->regs.esp,
    //     proc->thread.kernel_stack_ptr,
    //     proc->regs.cr3);

    tss_set_ring0_stack(0x10, proc->thread.kernel_stack_ptr);
    enter_usermode(proc->thread.instr_ptr, proc->thread.user_stack_ptr);
}

void process_create_idle(void (*main)())
{
    //Only to be run ONCE!
    if(_current_process == 0)
    {
        process_t* idleproc = (process_t*)kmalloc(sizeof(process_t));
        idleproc->pagedir = vmm2_get_directory();
        idleproc->next = idleproc; //loop
        idleproc->pid = num_proc;

        idleproc->thread.user_stack_ptr = 0;
        //16 byte stack, starts at location 16-4 = 12
        idleproc->thread.kernel_stack_ptr = (unsigned int)kmalloc(4096) + 4092;
        idleproc->thread.instr_ptr = (unsigned int)main;
        idleproc->thread.priority = 0;
        idleproc->thread.state = 0;

        idleproc->regs.eax = 0;
        idleproc->regs.ebx = 0;
        idleproc->regs.ecx = 0;
        idleproc->regs.edx = 0;
        idleproc->regs.esi = 0;
        idleproc->regs.edi = 0;
        idleproc->regs.eip = idleproc->thread.instr_ptr;
        idleproc->regs.cr3 = (unsigned int)idleproc->pagedir->tables;
        idleproc->regs.esp = idleproc->thread.kernel_stack_ptr;
        __asm("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(idleproc->regs.eflags)::"%eax");

        _processes[num_proc++] = idleproc;
        _current_process = idleproc;

        // printk("IDLE proc %x - EIP %x ESP: %x kESP: %x CR3: %x\n\n",
        //     idleproc,
        //     idleproc->regs.eip,
        //     idleproc->regs.esp,
        //     idleproc->thread.kernel_stack_ptr,
        //     idleproc->regs.cr3);
    }
    else
    {
        //PANIC!
    }
}
