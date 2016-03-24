// Process.h
// Multiasking functionality

#ifndef INCLUDE_TROS_TASK_H
#define INCLUDE_TROS_TASK_H

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

typedef struct process
{
    registers_t regs;
    struct process *next;
} process_t;

void process_init();
extern void process_switch(registers_t* old, registers_t* new);
void process_create(process_t* task, void(*main)(), unsigned int flags, unsigned int* pagedir);
void process_preempt();

#endif
