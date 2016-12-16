// timer.h
// Timer used for preemptive scheduling

#ifndef INCLUDE_TROS_TIMER_H
#define INCLUDE_TROS_TIMER_H

void timer_initialize(unsigned int frequency);
void timer_sleep(unsigned int wait);

#endif
