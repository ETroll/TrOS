#include <tros/process.h>
#include <tros/memory.h>
#include <tros/tros.h>
#include <tros/hal/tss.h>
#include <tros/klib/list.h>
#include <tros/klib/kstring.h>
#include <tros/elf.h>

static thread_t* _current_thread = 0;
static list_t* _threads = 0;
static list_t* _processes = 0;

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
        proc->heapendAddr = PROCESS_MEM_START;
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
        if(proc == process_getCurrent())
        {
            if(proc->threads)
            {
                foreach(node, proc->threads)
                {
                    thread_setState((thread_t*)node->data, THREAD_DISPOSING);
                }
            }
            scheduler_reschedule();
        }
        else
        {
            if(proc->threads)
            {
                foreach(node, proc->threads)
                {
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
            if(proc->argc > 0 && proc->argv)
            {
                for(int i = 0; i<proc->argc; i++)
                {
                    kfree(proc->argv[i]);
                }
                kfree(proc->argv);
            }
            kfree(proc);
        }
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
        scheduler_addThread(thread);

        if(proc->pid == 0)
        {
            _current_thread = thread;
            process_startIdle(thread->instrPtr, thread->kernelStackPtr);
        }
    }
    return proc;
}

process_t* process_executeUser(int argc, char** argv)
{
    process_t* proc = process_create();
    if(proc)
    {
        if(proc->parent)
        {
            list_add(proc->parent->children, proc);
        }
        thread_t* thread = thread_create(proc, (uint32_t)process_initalizeNewProcess, TFLAG_USER);
        scheduler_addThread(thread);

        if(argc > 0)
        {
            proc->argv = (char**)kmalloc(sizeof(char*)*argc);
            for(int i = 0; i<argc; i++)
            {
                proc->argv[i] = (char*)kmalloc((sizeof(char) * strlen(argv[i]))+1);
                strcpy(proc->argv[i], argv[i]);
            }
        }
    }
    return proc;
}

process_t* process_getCurrent()
{
    return _current_thread->process;
}

process_t* process_getFromPid(uint32_t pid)
{
    process_t* process = 0;
    if(_processes)
    {
        foreach(node, _processes)
        {
            process_t* proc = (process_t*)node->data;
            if(proc->pid == pid)
            {
                process = proc;
                break;
            }
        }
    }
    return process;
}

void process_initalizeNewProcess()
{
    thread_t* thread = thread_getCurrent();
    printk("Process Init procedure called for PID: %d\n", thread->process->pid);
    if(thread->process->argc > 0)
    {
        printk("Executing file: %s with %d arguments CR3 %x\n",
            thread->process->argv[0],
            thread->process->argc,
            vmm2_get_directory());

        thread->process->heapendAddr = elf32_load(thread->process->argv[0], &thread->instrPtr);

        if(thread->instrPtr > 0 && thread->process->heapendAddr > PROCESS_MEM_START)
        {
            //TODO: Preload userland stack with arguments!
            thread_enterUsermode(0,
                thread->instrPtr,
                thread->userStackPtr);
        }
        else
        {
            printk("ERROR! instrPtr: %x heapendAddr: %x\n",
                thread->instrPtr,
                thread->process->heapendAddr);
        }
    }
    //We have somehow failed, lets kill the process
    process_dispose(thread->process);
}

thread_t* thread_create(process_t* parent, uint32_t instrPointer, thread_flag_t flags)
{
    thread_t* thread = (thread_t*)kmalloc(sizeof(thread_t));
    if(thread)
    {
        uint32_t userStackAddr = 0;
        if(flags & TFLAG_USER)
        {
            //TODO: Increase to 16K
            //TODO: Figure out next stack position, now all threads use same stack.
            //      And that is BAD, and will only work for 1 thread
            //      Account for removed/disposed threads!
            userStackAddr = 0xBFFFC000; //16k below kernel
            vmm2_map_todir(userStackAddr, 1, VMM2_PAGE_USER | VMM2_PAGE_WRITABLE, parent->pagedir);
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
        thread->regs.esp = thread->kernelStackPtr;

        //TODO: Replace with abstraction for compatability
        __asm("pushfl; movl (%%esp), %%eax; movl %%eax, %0; popfl;":"=m"(thread->regs.eflags)::"%eax");

        list_add(parent->threads, thread);
    }
    return thread;
}

void thread_dispose(thread_t* thread)
{
    if(thread)
    {
        //TODO: Check if current running thread
        //      If so, then mark as DISPOSING and reschedule
        //TODO: Unmap userland stack
        if(thread == thread_getCurrent())
        {
            thread_setState(thread, THREAD_DISPOSING);
            scheduler_reschedule();
        }
        else
        {
            scheduler_removeThread(thread);
            if(thread->kernelStackPtr)
            {
                kfree((void*)thread->kernelStackPtr);
            }
            kfree(thread);
        }
    }
}

thread_t* thread_getCurrent()
{
    return _current_thread;
}

thread_t* thread_getFromTid(uint32_t tid)
{
    thread_t* thread = 0;
    if(_threads)
    {
        foreach(node, _threads)
        {
            thread_t* th = (thread_t*)node->data;
            if(th->tid == tid)
            {
                thread = th;
                break;
            }
        }
    }
    return thread;
}

void thread_setState(thread_t* p, thread_state_t s)
{
    p->state = s;
    if(s == THREAD_WAITIO || s == THREAD_SLEEPING)
    {
        printk("TID %d waiting/sleeping\n", p->tid);
    }
}


void scheduler_initialize()
{
    _threads = list_create();
    _processes = list_create();
}

thread_t* scheduler_findNextThread()
{
    //TODO: Complete!
    return 0;
}

void scheduler_reschedule()
{
    thread_t* next = scheduler_findNextThread();

    tss_set_ring0_stack(0x10, next->kernelStackPtr);

    thread_t *prev = _current_thread;
    _current_thread = next;
    _current_thread->state = THREAD_RUNNING;

    // printk("Switching to TID %d\n", _current_thread->tid);
    // printk("             EIP %x\n", _current_thread->regs.eip);
    // printk("             ESP %x\n", _current_thread->regs.esp);
    // printk("          Kstack %x (%d bytes used)\n", _current_thread->kernelStackPtr, _current_thread->kernelStackPtr-_current_thread->regs.esp);
    // printk("             CR3 %x\n", _current_thread->regs.cr3);
    // printk("             CR3 %x\n", _current_thread->process->pagedir);

    scheduler_switchThread(&prev->regs, &next->regs);
}

void scheduler_addThread(thread_t* thread)
{
    if(thread)
    {
        uint8_t hasThread = 0;
        foreach(node, _threads)
        {
            if(node->data == thread)
            {
                hasThread = 1;
                break;
            }
        }
        if(!hasThread)
        {
            uint8_t hasProcess = 0;
            foreach(node, _processes)
            {
                if(node->data == thread->process)
                {
                    hasProcess = 1;
                    break;
                }
            }
            if(!hasProcess)
            {
                list_add(_processes, thread->process);
            }
            list_add(_threads, thread);
        }
    }
}

void scheduler_removeThread(thread_t* thread)
{
    //TODO: If the threds process does not have any more threads, remove it
    //      from _processes
    if(thread)
    {
        if(thread->process->threads->size == 1)
        {

        }
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

*/
