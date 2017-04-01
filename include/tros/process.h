// Process.h
// Multiasking functionality

#ifndef INCLUDE_TROS_TASK_H
#define INCLUDE_TROS_TASK_H

#include <stdint.h>
#include <tros/mem/vmm2.h>
#include <tros/mailbox.h>

#define PROCESS_MEM_START 0x200000

typedef enum {
    PROCESS_RUNNING = 0x1,
    PROCESS_WAITIO,
    PROCESS_IOREADY,
    PROCESS_SLEEPING
} process_state_t;

typedef struct
{
    unsigned int eax; // 0
    unsigned int ebx; // 4
    unsigned int ecx; // 8
    unsigned int edx; // 12
    unsigned int esi; // 16
    unsigned int edi; // 20
    unsigned int esp; // 24
    unsigned int ebp; // 28
    unsigned int eip; // 32
    unsigned int eflags; // 36
    unsigned int cr3; // 40
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
    struct process *parent;
    struct process *next;
    mailbox_t* mailbox;
    uint32_t pid;
    uint32_t heapend_addr;
    uint8_t started;
} process_t;

extern void process_switch(registers_t* old, registers_t* new);
extern void process_start_idle(uint32_t eip, uint32_t kesp);


void process_preempt();
void process_switchto(process_t* next);
void process_create_idle(void (*main)());

uint32_t process_exec_user(uint32_t startAddr, uint32_t ustack, uint32_t heapstart, uint32_t kstack, page_directory_t* pdir);
uint32_t process_exec(uint32_t startAddr, uint32_t ustack, uint32_t kstack, page_directory_t* pdir);
void process_dispose(process_t* p);

process_t* process_get_current();
process_t* process_get_pid(uint32_t pid);

//Set the state of the current running process and rescedule if needed
void process_set_state(process_t* p, process_state_t s);

#endif
