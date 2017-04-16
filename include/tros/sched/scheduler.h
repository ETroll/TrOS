
#ifndef INCLUDE_TROS_SCHEDULER_H
#define INCLUDE_TROS_SCHEDULER_H

#include <tros/sched/process.h>
#include <tros/sched/thread.h>

void scheduler_reschedule();
void scheduler_initialize();
void scheduler_addThread(thread_t* thread);
void scheduler_removeThread(thread_t* thread);
void scheduler_removeProcess(process_t* thread);

process_t* scheduler_executeKernel(int (*main)());
process_t* scheduler_executeUser(int argc, char** argv);
process_t* scheduler_getCurrentProcess();
process_t* scheduler_getCurrentProcessFromPid(uint32_t pid);
thread_t* scheduler_getCurrentThread();
thread_t* scheduler_getCurrentThreadFromTid(uint32_t tid);

#endif
