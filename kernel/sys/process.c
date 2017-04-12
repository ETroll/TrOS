#include <tros/process.h>
#include <tros/memory.h>
#include <tros/tros.h>
#include <tros/hal/tss.h>
#include <tros/klib/list.h>
#include <tros/klib/kstring.h>
#include <tros/elf.h>

static thread_t* _current_thread = 0;

static list_t* _threads;
static unsigned int _nextpid = 0;
static unsigned int _nexttid = 0;

static void process_initalizeNewProcess();

static thread_t* scheduler_findNextThread();

extern void thread_enterUsermode(registers_t* reg, unsigned int location, unsigned int userstack);


process_t* process_create()
{
    process_t* proc = (process_t*)kmalloc(sizeof(process_t));
    if(proc)
    {
        proc->threads = list_create();
        proc->children = list_create();
        proc->pagedir = _nextpid == 0 ? vmm2_get_directory() : vmm2_create_directory();
        proc->parent = process_getCurrent();
        proc->mailbox = mailbox_create();
        proc->heapend_addr = PROCESS_MEM_START;
        proc->pid = _nextpid++;
        proc->argc = 0;
        proc->argv = 0;
    }
    return proc;
}

void process_dispose(process_t* proc)
{
    //NOTE: Dont think this will work well if we
    //      dispose the current running process.
    //
    //      If current process == proc
    //          Set all threads to THREAD_DISPOSING
    //          Add process to dispose queue
    //      else
    //          Dispose as usual
    if(proc)
    {
        if(proc->threads)
        {
            foreach(node, proc->threads)
            {
                scheduler_removeThread((thread_t*)node->data);
                thread_dispose((thread_t*)node->data);
            }
            list_dispose(proc->threads);
        }
        if(proc->children)
        {
            foreach(node, proc->children)
            {
                process_dispose((process_t*)node->data);
            }
            list_dispose(proc->children);
        }
        if(proc->pagedir)
        {
            vmm2_dispose_directory(proc->pagedir);
        }
        if(proc->mailbox)
        {
            mailbox_dispose(proc->mailbox);
        }
        kfree(proc);
    }
}

process_t* process_executeKernel(int (*main)())
{
    process_t* proc = process_create();
    if(proc)
    {
        if(proc->parent)
        {
            list_add(proc->parent->children, proc);
        }
        thread_t* thread = thread_create(proc, (uint32_t)main, TFLAG_KERNEL);
        list_add(proc->threads, thread);
        scheduler_addThread(thread);

        if(proc->pid == 0)
        {
            _current_thread = thread;
            process_startIdle(thread.instrPtr, thread.kernelStackPtr);
        }
    }
    return proc;
}

process_t* process_executeUser(int argc, char** argv)
{

}


thread_t* thread_create(process_t* parent, uint32_t instrPointer, thread_flag_t flags)
{
    thread_t* thread = (thread_t*)kmalloc(sizeof(thread_t));
    if(thread)
    {
        uint32_t userStackAddr = 0;
        uint32_t eflags = 0;
        if(flags & TFLAG_USER)
        {
            //TODO: Increase to 16K
            //TODO: Figure out next stack position, now all threads use same stack.
            //      And that is BAD, and will only work for 1 thread
            //      Account for removed/disposed threads!
            userStackAddr = 0xBFFFC000; //16k below kernel
            vmm2_map_todir(userStackAddr, 1, VMM2_PAGE_USER | VMM2_PAGE_WRITABLE, proc->pagedir);
            userStackAddr += (VMM2_BLOCK_SIZE - sizeof(unsigned int));
        }

        thread->kernelStackPtr = (uint32_t)kmalloc(KERNEL_STACK_SIZE) + (KERNEL_STACK_SIZE - sizeof(uint32_t));
        thread->userStackPtr = userStackAddr;
        thread->instrPtr = instrPointer;
        thread->process = parent;
        thread->state = THREAD_RUNNING;
        thread->priority = 50;
        thread->tid = _nexttid++;

        thread->regs.eax = 0;
        thread->regs.ebx = 0;
        thread->regs.ecx = 0;
        thread->regs.edx = 0;
        thread->regs.esi = 0;
        thread->regs.edi = 0;
        thread->regs.eip = instrPointer;
        thread->regs.cr3 = (uint32_t)parent->pagedir->tables;
        thread->regs.esp = kernelStackPtr;

        //TODO: Replace with abstraction for compatability
        __asm("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(thread->regs.eflags)::"%eax");
    }
    return thread;
}

void thread_dispose(thread_t* thread)
{
    if(thread)
    {
        //TODO: Unmap userland stack
        if(thread->kernelStackPtr)
        {
            kfree(thread->kernelStackPtr);
        }
        kfree(thread);
    }
}












/*
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
            if(itt->thread.state == THREAD_IOREADY)
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
                if(itt->thread.state == THREAD_RUNNING)
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
            // printk("Que?\n");
        }
    }
}

void process_switchTo(process_t* next)
{
    tss_set_ring0_stack(0x10, next->thread.kernel_stack_ptr);

    process_t *prev = _current_process;
    _current_process = next;
    _current_process->thread.state = THREAD_RUNNING;

    // printk("Switching to PID %d\n", _current_process->pid);
    // printk("             EIP %x\n", _current_process->regs.eip);
    // printk("             ESP %x\n", _current_process->regs.esp);
    // printk("          Kstack %x (%d bytes used)\n", _current_process->thread.kernel_stack_ptr, _current_process->thread.kernel_stack_ptr-_current_process->regs.esp);
    // printk("             CR3 %x\n", _current_process->regs.cr3);
    // printk("             CR3 %x\n", _current_process->pagedir);

    process_switch(&prev->regs, &_current_process->regs);
}

uint32_t process_create(int argc, char** argv)
{
    process_t* proc = (process_t*)kmalloc(sizeof(process_t));
    if(proc)
    {
        proc->pagedir = vmm2_create_directory();
        proc->pid = num_proc;
        proc->parent = process_get_current();
        proc->mailbox = mailbox_create();
        proc->heapend_addr = PROCESS_MEM_START;
        proc->next_tid = 0;

        //Set up "round robin scheduling"
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
        _processes[num_proc++] = proc;

        // Create a thread. (TODO: make a thread_create method)
        // Stack is 1 block - 4K stack (TODO Increase to 16k?)
        uint32_t ustackAddr = 0xBFFFC000; //16k below kernel
        vmm2_map_todir(ustackAddr, 1, VMM2_PAGE_USER | VMM2_PAGE_WRITABLE, proc->pagedir);
        ustackAddr += (VMM2_BLOCK_SIZE - sizeof(unsigned int));

        // Allocate some space to the kernel stack
        uint32_t kstackAddr = (uint32_t)kmalloc(KERNEL_STACK_SIZE) + (KERNEL_STACK_SIZE - sizeof(uint32_t));

        proc->thread.user_stack_ptr = ustackAddr;
        proc->thread.kernel_stack_ptr = kstackAddr;
        proc->thread.instr_ptr = (uint32_t)process_init;
        proc->thread.priority = 1;
        proc->thread.state = THREAD_RUNNING;

        //Initial register values
        proc->regs.eax = 0;
        proc->regs.ebx = 0;
        proc->regs.ecx = 0;
        proc->regs.edx = 0;
        proc->regs.esi = 0;
        proc->regs.edi = 0;
        proc->regs.eip = proc->thread.instr_ptr;
        proc->regs.cr3 = (unsigned int)proc->pagedir->tables;
        proc->regs.esp = proc->thread.kernel_stack_ptr;     //kstack, since we are starting in the kernel
        proc->regs.eflags = _current_process->regs.eflags;

        //Set up and store initial arguments
        proc->argc = argc;
        proc->argv = 0;
        if(argc > 0)
        {
            proc->argv = (char**)kmalloc(sizeof(char*)*argc);
            for(int i = 0; i<argc; i++)
            {
                proc->argv[i] = (char*)kmalloc((sizeof(char) * strlen(argv[i]))+1);
                strcpy(proc->argv[i], argv[i]);
            }
        }
        return proc->pid;
    }
    else
    {
        return -1;
    }

}

void process_createIdle(void (*main)())
{
    //Only to be run ONCE!
    if(_current_process == 0)
    {
        process_t* idleproc = (process_t*)kmalloc(sizeof(process_t));
        // printk("Process PID %d at %x (%x)\n", num_proc, idleproc, main);
        idleproc->pagedir = vmm2_get_directory();
        idleproc->next = idleproc; //loop
        idleproc->pid = num_proc;
        idleproc->parent = 0;
        idleproc->mailbox = 0;
        idleproc->heapend_addr = PROCESS_MEM_START;
        idleproc->argc = 0;
        idleproc->argv = 0;
        idleproc->next_tid = 0;

        idleproc->thread.user_stack_ptr = 0;
        //16 kbyte stack, starts at location 16-4 = 12
        idleproc->thread.kernel_stack_ptr = (unsigned int)kmalloc(4096) + 4092;
        idleproc->thread.instr_ptr = (unsigned int)main;
        idleproc->thread.priority = 0;
        idleproc->thread.state = THREAD_RUNNING;

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

process_t* process_getCurrent()
{
    return _current_process;
}

process_t* process_getPid(uint32_t pid)
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

void process_setState(process_t* p, thread_state_t s)
{
    p->thread.state = s;
    if(s == THREAD_WAITIO || s == THREAD_SLEEPING)
    {
        printk("PID %d waiting/sleeping\n", p->pid);
        process_preempt();
    }
}

void process_dispose(process_t* p)
{
    process_set_state(p, THREAD_SLEEPING);
}

void process_init()
{
    process_t* proc = process_get_current();
    printk("Process Init procedure called for PID: %d\n", proc->pid);
    if(proc->argc > 0)
    {
        printk("Executing file: %s with %d arguments CR3 %x\n",
            proc->argv[0],
            proc->argc,
            vmm2_get_directory());

        proc->heapend_addr = elf32_load(proc->argv[0], &proc->thread.instr_ptr);

        if(proc->thread.instr_ptr > 0 && proc->heapend_addr > PROCESS_MEM_START)
        {
            //TODO: Preload userland stack with arguments!
            enter_usermode(0,
                proc->thread.instr_ptr,
                proc->thread.user_stack_ptr);
        }
        else
        {
            printk("ERROR! instr_ptr: %x heapend_addr: %x\n",
                proc->thread.instr_ptr,
                proc->heapend_addr);
        }
    }
    //We have somehow failed, lets kill the process
    process_dispose(proc);
}
*/
