#include <tros/sched/scheduler.h>
#include <tros/tros.h>
#include <tros/hal/tss.h>
#include <tros/memory.h>
#include <tros/klib/kstring.h>
#include <tros/elf.h>

typedef struct schedule_node
{
    struct schedule_node* next;
    struct schedule_node* prev;
    thread_t* thread;
} schedule_node_t;

typedef struct
{
    schedule_node_t* current;
    schedule_node_t* head;
    uint32_t size;
    list_t* threads;
    list_t* processes;
} scheduler_roundrobin_t;

static scheduler_roundrobin_t* _scheduler = 0;


static schedule_node_t* scheduler_findNextThread();
static void scheduler_initalizeNewProcess();

extern void scheduler_switchThread(registers_t* prev, registers_t* next);
extern void scheduler_startIdle(uint32_t eip, uint32_t kesp);
extern void thread_enterUsermode(registers_t* reg, unsigned int location, unsigned int userstack);

void scheduler_initialize()
{
    _scheduler = (scheduler_roundrobin_t*)kmalloc(sizeof(scheduler_roundrobin_t));
    if(_scheduler)
    {
        _scheduler->current = 0;
        _scheduler->head = 0;
        _scheduler->size = 0;
        _scheduler->threads = list_create();
        _scheduler->processes = list_create();
    }
    else
    {
        kernel_panic("Could not set up scheduler!", 0);
    }
}

schedule_node_t* scheduler_findNextThread()
{
    schedule_node_t* next = 0;
    for(schedule_node_t* itt = _scheduler->current->next;
        itt != _scheduler->current && next == 0;
        itt = itt->next)
    {
        if(itt->thread->state == THREAD_IOREADY)
        {
            next = itt;
            break;
        }
    }
    if(next == 0)
    {
        for(schedule_node_t* itt = _scheduler->current->next;
            itt != _scheduler->current && next == 0;
            itt = itt->next)
        {
            if(itt->thread->state == THREAD_RUNNING)
            {
                next = itt;
                break;
            }
        }
        if(next == 0)
        {
            //OK! No other thread needs to be run.. default to idle
            next = _scheduler->head;
        }
    }
    return next;
}

void scheduler_reschedule()
{
    if(_scheduler && _scheduler->size > 1)
    {
        schedule_node_t* next = scheduler_findNextThread();
        if(next)
        {
            tss_set_ring0_stack(0x10, next->thread->kernelStackPtr);

            schedule_node_t *prev = _scheduler->current;;
            next->thread->state = THREAD_RUNNING;
            _scheduler->current = next;

            // printk("Switching to TID %d\n", _scheduler->current->thread->tid);
            // printk("             EIP %x\n", _scheduler->current->thread->regs.eip);
            // printk("             ESP %x\n", _scheduler->current->thread->regs.esp);
            // printk("          Kstack %x (%d bytes used)\n", _scheduler->current->thread->kernelStackPtr, _scheduler->current->thread->kernelStackPtr-_scheduler->current->thread->regs.esp);
            // printk("             CR3 %x\n", _scheduler->current->thread->regs.cr3);
            // printk("             CR3 %x\n", _scheduler->current->thread->process->pagedir);

            scheduler_switchThread(&prev->thread->regs, &next->thread->regs);
        }
        else
        {
            kernel_panic("No task was found by the scheduler!", 0);
        }
    }
}

void scheduler_addThread(thread_t* thread)
{
    if(thread)
    {
        uint8_t hasThread = 0;
        foreach(node, _scheduler->threads)
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
            foreach(node, _scheduler->processes)
            {
                if(node->data == thread->process)
                {
                    hasProcess = 1;
                    break;
                }
            }
            if(!hasProcess)
            {
                list_add(_scheduler->processes, thread->process);
            }
            list_add(_scheduler->threads, thread);

            schedule_node_t* node = (schedule_node_t*)kmalloc(sizeof(schedule_node_t));
            node->thread = thread;
            node->next = 0;
            node->prev = 0;

            if(_scheduler->current == 0)
            {
                if(thread->process->pid == 0)
                {
                    node->next = node;
                    node->prev = node;
                    _scheduler->current = node;
                    _scheduler->head = node;
                    _scheduler->size = 1;
                }
                else
                {
                    kernel_panic("Fist thread in scheduler is not IDLE thread! Aborting!", 0);
                }
            }
            else
            {
                node->next = _scheduler->head;
                node->prev = _scheduler->head->prev;
                _scheduler->head->prev->next = node;
                _scheduler->head->prev = node;
                _scheduler->size++;
            }
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
    printk("Disposing PID %d with %d threads and %d subprocesses\n",
            proc->pid, proc->threads->size, proc->children->size);
    if(proc->threads && proc->threads->size > 0)
    {
        foreach(node, proc->threads)
        {
            thread_setState((thread_t*)node->data, THREAD_DISPOSING);
        }
    }
    if(proc->children && proc->children->size > 0)
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
        thread_t* thread = thread_create(proc, (uint32_t)main, TFLAG_KERNEL);
        scheduler_addThread(thread);

        if(proc->pid == 0)
        {
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
        thread_t* thread = thread_create(proc, (uint32_t)scheduler_initalizeNewProcess, TFLAG_USER);
        scheduler_addThread(thread);

        if(argc > 0)
        {
            proc->argc = argc;
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
    if(_scheduler->current)
    {
        return _scheduler->current->thread->process;
    }
    else
    {
        return 0;
    }
}

process_t* scheduler_getProcessFromPid(uint32_t pid)
{
    process_t* process = 0;
    if(_scheduler->processes)
    {
        foreach(node, _scheduler->processes)
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
    return _scheduler->current->thread;
}

thread_t* scheduler_getThreadFromTid(uint32_t tid)
{
    thread_t* thread = 0;
    if(_scheduler->threads)
    {
        foreach(node, _scheduler->threads)
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
    printk("Process Init procedure called for PID: %d with %d arguments\n",
        thread->process->pid, thread->process->argc);
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
    printk("scheduler_initalizeNewProcess: FAILED!\n");
    scheduler_removeProcess(thread->process);
    scheduler_reschedule();
}
