#include <tros/sched/thread.h>
#include <tros/klib/list.h>
#include <tros/tros.h>
#include <tros/memory.h>

static unsigned int _nexttid = 0;

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
        thread->flags = flags;
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
        //TODO: Unmap userland stack
        printk("thread_dispose: LEAKING! Stack not unmapped!\n");
        if(thread->kernelStackPtr)
        {
            kfree((void*)thread->kernelStackPtr);
        }
        kfree(thread);
    }
}

void thread_setState(thread_t* p, thread_state_t s)
{
    p->state = s;
    if(s == THREAD_WAITIO || s == THREAD_SLEEPING)
    {
        printk("TID %d waiting/sleeping\n", p->tid);
    }
}
