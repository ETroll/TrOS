// Process.h
// Multiasking functionality

#ifndef INCLUDE_TROS_TASK_H
#define INCLUDE_TROS_TASK_H

#include <stdint.h>
#include <tros/mem/vmm2.h>


#define PROCESS_MEM_START 0x200000

typedef enum {
    PROCESS_RUNNING = 0x1,
    PROCESS_WAITIO,
    PROCESS_IOREADY,
    PROCESS_SLEEPING
} process_state_t;

typedef struct
{
    unsigned int eax;
    unsigned int ebx;
    unsigned int ecx;
    unsigned int edx;
    unsigned int esi;
    unsigned int edi;
    unsigned int esp;
    unsigned int ebp;
    unsigned int eip;
    unsigned int eflags;
    unsigned int cr3;
} __attribute__((packed)) registers_t;

typedef struct
{
    unsigned int user_stack_ptr;
    unsigned int kernel_stack_ptr;
    unsigned int instr_ptr;
    int priority;
    process_state_t state;
} thread_t;

typedef struct process
{
    thread_t thread;
    registers_t regs;
    page_directory_t* pagedir;
    struct process *next;
    uint32_t pid;
    uint32_t heapend_addr;
} process_t;

// void process_init();
extern void process_switch(registers_t* old, registers_t* new);
// void process_create(process_t* task, void(*main)(), unsigned int flags, unsigned int* pagedir);

void process_preempt();
void process_switchto(process_t* next);
void process_create_idle(void (*main)());

// void process_exec_user(unsigned int startAddr);
void process_exec_user(uint32_t startAddr, uint32_t ustack, uint32_t heapstart, uint32_t kstack, page_directory_t* pdir);

process_t* process_get_current();

//Set the state of the current running process and rescedule if needed
void process_set_state(process_t* p, process_state_t s);

#endif
