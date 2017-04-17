
#include <tros/sched/process.h>
#include <tros/sched/thread.h>
#include <tros/memory.h>

static unsigned int _nextpid = 0;


process_t* process_create(process_t* parent)
{
    process_t* proc = (process_t*)kmalloc(sizeof(process_t));
    if(proc)
    {
        proc->threads = list_create();
        proc->children = list_create();
        proc->pagedir = _nextpid == 0 ? vmm2_get_directory() : vmm2_create_directory();
        proc->parent = parent;
        proc->mailbox = mailbox_create();
        proc->heapendAddr = PROCESS_MEM_START;
        proc->pid = _nextpid++;
        proc->name = 0;
        proc->argc = 0;
        proc->argv = 0;

        if(parent)
        {
            list_add(parent->children, proc);
        }
    }
    return proc;
}

void process_dispose(process_t* proc)
{
    if(proc)
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
