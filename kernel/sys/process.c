#include <tros/process.h>
#include <tros/memory.h>
#include <tros/tros.h>
#include <tros/hal/tss.h>


#define PROC_STACK_SIZE 16384 //16K stack?

static process_t* _current_process = 0;

static process_t* _processes[10]; //just 10 while testing..
static unsigned int num_proc = 0;

extern void enter_usermode(registers_t* reg, unsigned int location, unsigned int userstack);

//TODO: Rename to process_rescedule or something
void process_preempt()
{
    if(_current_process != 0 && num_proc > 1)
    {
        // printk("X");
        process_t* next = 0;
        for(process_t* itt = _current_process->next;
            itt != _current_process && next == 0;
            itt = itt->next)
        {
            if(itt->thread.state == PROCESS_IOREADY)
            {
                next = itt;
                break;
            }
        }
        // printk("X");
        if(next == 0)
        {
            for(process_t* itt = _current_process->next;
                itt != _current_process && next == 0;
                itt = itt->next)
            {
                if(itt->thread.state == PROCESS_RUNNING)
                {
                    // printk("X");
                    next = itt;
                }
            }
        }

        if(next != 0)
        {
            // printk("Y");
            process_switchto(next);
        }
    }
}

void process_switchto(process_t* next)
{
    tss_set_ring0_stack(0x10, next->thread.kernel_stack_ptr);

    process_t *prev = _current_process;
    _current_process = next;
    _current_process->thread.state = PROCESS_RUNNING;
    if(_current_process->started)
    {
        printk(" Switching to PID %d\n", _current_process->pid);
        printk("              EIP %x\n", _current_process->regs.eip);
        printk("              ESP %x\n", _current_process->regs.esp);
        process_switch(&prev->regs, &_current_process->regs);
    }
    else
    {
        printk(" Saving state for PID: %d\n", prev->pid);
        printk("                  EIP: %x\n", prev->regs.eip);
        printk("                  ESP: %x\n", prev->regs.esp);
        // process_savestate(&prev->regs);
        printk(" Starting PID %d for the first time\n", _current_process->pid);
        _current_process->started = 1;
        // tss_set_ring0_stack(0x10, next->thread.kernel_stack_ptr);
        enter_usermode(&prev->regs,
            _current_process->thread.instr_ptr,
            _current_process->thread.user_stack_ptr);
    }
}

uint32_t process_exec_user(uint32_t startAddr, uint32_t ustack, uint32_t heapstart, uint32_t kstack, page_directory_t* pdir)
{
    process_t* proc = (process_t*)kmalloc(sizeof(process_t));
    printk("Process PID %d at %x\n", num_proc, proc);
    proc->pagedir = pdir;
    proc->pid = num_proc;
    proc->parent = process_get_current();
    proc->mailbox = mailbox_create();
    proc->heapend_addr = heapstart;
    proc->started = 0;

    if(num_proc > 0)
    {
        process_t* prev = _processes[num_proc-1];
        proc->next = _processes[0];
        prev->next = proc;
    }
    else
    {
        proc->next = proc; //loop
    }

    proc->thread.user_stack_ptr = ustack;
    proc->thread.kernel_stack_ptr = kstack;

    proc->thread.instr_ptr = startAddr;
    proc->thread.priority = 1;
    proc->thread.state = PROCESS_RUNNING;

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
    return proc->pid;
}

void process_create_idle(void (*main)())
{
    //Only to be run ONCE!
    if(_current_process == 0)
    {
        process_t* idleproc = (process_t*)kmalloc(sizeof(process_t));
        printk("Process PID %d at %x (%x)\n", num_proc, idleproc, main);
        idleproc->pagedir = vmm2_get_directory();
        idleproc->next = idleproc; //loop
        idleproc->pid = num_proc;
        idleproc->parent = 0;
        idleproc->started = 1;
        idleproc->mailbox = 0;
        idleproc->heapend_addr = PROCESS_MEM_START;

        idleproc->thread.user_stack_ptr = 0;
        //16 kbyte stack, starts at location 16-4 = 12
        idleproc->thread.kernel_stack_ptr = (unsigned int)kmalloc(4096) + 4092;
        idleproc->thread.instr_ptr = (unsigned int)main;
        idleproc->thread.priority = 0;
        idleproc->thread.state = PROCESS_RUNNING;

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
        // process_switchto(_current_process);

        process_start_idle(idleproc->thread.instr_ptr, idleproc->thread.kernel_stack_ptr);
        printk("You should never see me!\n");
    }
    else
    {
        kernel_panic("Tried to start the IDLE PROCESS twice!", 0);
    }
}

process_t* process_get_current()
{
    return _current_process;
}

process_t* process_get_pid(uint32_t pid)
{
    process_t* proc = 0;
    for(process_t* itt = _current_process->next;
        itt != _current_process && proc == 0;
        itt = itt->next)
    {
        if(itt->pid == pid)
        {
            proc = itt;
            break;
        }
    }
    return proc;
}

void process_set_state(process_t* p, process_state_t s)
{
    p->thread.state = s;
    if(s == PROCESS_WAITIO || s == PROCESS_SLEEPING)
    {
        printk("PID %d waiting/sleeping\n", p->pid);
        process_preempt();
    }
}

void process_dispose(process_t* p)
{
    process_set_state(p, PROCESS_SLEEPING);
}
