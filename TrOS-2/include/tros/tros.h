#ifndef INCLUDE_KERNEL_H
#define INCLUDE_KERNEL_H

#include <tros/irq.h>
//#define GenerateInterrupt(arg) __asm__("int %0\n" : : "N"((arg)) : "cc", "memory")
#define BOCHS_DEBUG __asm__("xchgw %bx, %bx");

//temp
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;

//void printk_initialize(void (*putch)(char));
void printk(char* str, ...);

void kernel_panic(const char* message, cpu_registers_t* regs);

#endif
