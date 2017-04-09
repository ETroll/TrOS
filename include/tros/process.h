// Process.h
// Multiasking functionality

#ifndef INCLUDE_TROS_TASK_H
#define INCLUDE_TROS_TASK_H

#include <stdint.h>
#include <tros/mem/vmm2.h>
#include <tros/mailbox.h>

#define PROCESS_MEM_START 0x400000

typedef enum {
    THREAD_RUNNING = 0x1,
    THREAD_WAITIO,
    THREAD_IOREADY,
    THREAD_SLEEPING
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

typedef struct
{
    uint32_t tid;
    uint32_t user_stack_ptr;
    uint32_t kernel_stack_ptr;
    uint32_t instr_ptr;
    int priority;
    thread_state_t state;
} thread_t;

typedef struct process
{
    thread_t thread;
    registers_t regs;
    page_directory_t* pagedir;
    struct process *parent;
    struct process *next;
    mailbox_t* mailbox;
    uint32_t pid;
    uint32_t heapend_addr;
    char** argv;
    int argc;
    uint32_t next_tid;
} process_t;

extern void process_switch(registers_t* old, registers_t* new);
extern void process_start_idle(uint32_t eip, uint32_t kesp);


void process_preempt();
void process_switchto(process_t* next);
void process_create_idle(void (*main)());

uint32_t process_create(int argc, char** argv);
void process_dispose(process_t* p);

process_t* process_get_current();
process_t* process_get_pid(uint32_t pid);

//Set the state of the current running process and rescedule if needed
void process_set_state(process_t* p, thread_state_t s);

#endif
