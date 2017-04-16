#include <tros/sched/scheduler.h>
#include <tros/tros.h>
#include <tros/hal/tss.h>
#include <tros/memory.h>
#include <tros/klib/kstring.h>
#include <tros/elf.h>

static thread_t* _current_thread = 0;
static list_t* _threads = 0;
static list_t* _processes = 0;


static thread_t* scheduler_findNextThread();
static void scheduler_initalizeNewProcess();
extern void scheduler_switchThread(registers_t* prev, registers_t* next);
extern void scheduler_startIdle(uint32_t eip, uint32_t kesp);
extern void thread_enterUsermode(registers_t* reg, unsigned int location, unsigned int userstack);


void scheduler_initialize()
{
    _threads = list_create();
    _processes = list_create();
}

thread_t* scheduler_findNextThread()
{
    //TODO: Complete!
    if(_current_thread != 0 && _threads != 0 && _threads->size > 1)
    {
        printk("scheduler_findNextThread: NOT IMPLEMENTED\n");
    }
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
    if(thread)
    {
        thread_setState(thread, THREAD_DISPOSING);
    }
}

void scheduler_removeProcess(process_t* proc)
{
    if(proc->threads)
    {
        foreach(node, proc->threads)
        {
            thread_setState((thread_t*)node->data, THREAD_DISPOSING);
        }
    }
    if(proc->children)
    {
        foreach(node, proc->children)
        {
            scheduler_removeProcess((process_t*)node->data);
        }
    }
}

process_t* scheduler_executeKernel(int (*main)())
{
    process_t* proc = process_create(scheduler_getCurrentProcess());
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
            scheduler_startIdle(thread->instrPtr, thread->kernelStackPtr);
        }
    }
    return proc;
}

process_t* scheduler_executeUser(int argc, char** argv)
{
    process_t* proc = process_create(scheduler_getCurrentProcess());
    if(proc)
    {
        if(proc->parent)
        {
            list_add(proc->parent->children, proc);
        }
        thread_t* thread = thread_create(proc, (uint32_t)scheduler_initalizeNewProcess, TFLAG_USER);
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

process_t* scheduler_getCurrentProcess()
{
    if(_current_thread)
    {
        return _current_thread->process;
    }
    else
    {
        return 0;
    }
}

process_t* scheduler_getCurrentProcessFromPid(uint32_t pid)
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

thread_t* scheduler_getCurrentThread()
{
    return _current_thread;
}

thread_t* scheduler_getCurrentThreadFromTid(uint32_t tid)
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

void scheduler_initalizeNewProcess()
{
    thread_t* thread = scheduler_getCurrentThread();
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
    scheduler_removeProcess(thread->process);
}
