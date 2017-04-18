// timer.h
// Timer used for preemptive scheduling

#ifndef INCLUDE_TROS_TIMER_H
#define INCLUDE_TROS_TIMER_H

#include <tros/sched/scheduler.h>

void timer_initialize(uint32_t frequency);
void timer_sleep(thread_t* thread, uint32_t ticks);

#endif
