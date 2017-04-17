#ifndef INCLUDE_TROS_THREAD_H
#define INCLUDE_TROS_THREAD_H

#include <stdint.h>
#include <tros/sched/process.h>

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
    TFLAG_USER = 0x01,
    TFLAG_KERNEL = 0x02,
    TFLAG_NEEDKICK = 0x04
} thread_flag_t;

typedef struct
{
    uint32_t tid;
    uint32_t userStackPtr;
    uint32_t kernelStackPtr;
    uint32_t instrPtr;
    int priority;
    thread_state_t state;
    uint32_t flags;
    registers_t regs;
    process_t* process;
} thread_t;

thread_t* thread_create(process_t* parent, uint32_t instrPointer, thread_flag_t flags);
void thread_dispose(thread_t* thread);
void thread_setState(thread_t* p, thread_state_t s);

#endif
