#ifndef INCLUDE_TRLIB_THREAD_H
#define INCLUDE_TRLIB_THREAD_H

#include <stdint.h>

typedef unsigned int thread_t;

thread_t thread_start(void (*func)());
void thread_cancel(thread_t thread);
void thread_exit();
void thread_sleep(uint32_t ms);

#endif
