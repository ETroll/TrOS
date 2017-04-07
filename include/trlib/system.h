#ifndef INCLUDE_TRLIB_SYSTEM_H
#define INCLUDE_TRLIB_SYSTEM_H

#include <stdint.h>

typedef enum debug_flags
{
    DEBUG_NOP = 0xAABBCCDD,
    DEBUG_CR3 = 0x1
}debug_flags_t;


int system_debug(debug_flags_t flag);
int system_pid();
int system_parentpid();
int system_execute(char** params);
int system_kill(uint32_t pid);
void system_exit(int code);

#endif
