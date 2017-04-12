// Process.h
// Multiasking functionality

#ifndef INCLUDE_TROS_TASK_H
#define INCLUDE_TROS_TASK_H

#include <stdint.h>
#include <tros/mem/vmm2.h>
#include <tros/klib/list.h>
#include <tros/mailbox.h>

#define PROCESS_MEM_START 0x400000

typedef enum
{
    THREAD_RUNNING = 0x1,
    THREAD_WAITIO,
    THREAD_IOREADY,
    THREAD_SLEEPING,
    THREAD_DISPOSING
} thread_state_t;

typedef struct
{
    uint32_t eax; // 0
    uint32_t ebx; // 4
    uint32_t ecx; // 8
    uint32_t edx; // 12
    uint32_t esi; // 16
    uint32_t edi; // 20
    uint32_t esp; // 24
    uint32_t ebp; // 28
    uint32_t eip; // 32
    uint32_t eflags; // 36
    uint32_t cr3; // 40
} __attribute__((packed)) registers_t;

typedef enum
{
    TFLAG_USER = 0x00,
    TFLAG_KERNEL = 0x01
} thread_flag_t;

typedef struct process
{
    list_t* threads;
    list_t* children;
    page_directory_t* pagedir;
    struct process *parent;
    mailbox_t* mailbox;
    uint32_t pid;
    uint32_t heapend_addr;
    char** argv;
    int argc;
} process_t;

typedef struct
{
    uint32_t tid;
    uint32_t userStackPtr;
    uint32_t kernelStackPtr;
    uint32_t instrPtr;
    int priority;
    thread_state_t state;
    registers_t regs;
    process_t* process;
} thread_t;

//Process
extern void process_startIdle(uint32_t eip, uint32_t kesp);

process_t* process_create();
void process_dispose(process_t* proc);

process_t* process_executeUser(int argc, char** argv);
process_t* process_executeKernel(int (*main)());

process_t* process_getCurrent();
process_t* process_getFromPid(uint32_t pid);

//Thread
thread_t* thread_create(process_t* parent, uint32_t instrPointer, thread_flag_t flags);
void thread_dispose(thread_t* thread);
thread_t* thread_getCurrent();
thread_t* thread_getFromPid(uint32_t pid);
void thread_setState(thread_t* p, thread_state_t s);

//Scheduler
extern void scheduler_switchThread(registers_t* old, registers_t* new);
void scheduler_reschedule();
void scheduler_initialize();
void scheduler_addThread(thread_t* thread);
void scheduler_removeThread(thread_t* thread);


#endif
