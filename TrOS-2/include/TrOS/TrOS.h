#ifndef INCLUDE_KERNEL_H
#define INCLUDE_KERNEL_H

#include <TrOS/irq.h>
#define GenerateInterrupt(arg) __asm__("int %0\n" : : "N"((arg)) : "cc", "memory")

//void printk_initialize(void (*putch)(char));
void printk(char* str, ...);

void kernel_panic(const char* message, cpu_registers_t* regs);

#endif
