#ifndef INCLUDE_KERNEL_H
#define INCLUDE_KERNEL_H

#include <tros/irq.h>
#define GenerateInterrupt(arg) __asm__("int %0\n" : : "N"((arg)) : "cc", "memory")
#define BOCHS_DEBUG __asm__("xchgw %bx, %bx")


#define IOCTL_VGA_COLOR         1
#define IOCTL_VGA_SCROLL_UP     2
#define IOCTL_VGA_SCROLL_DOWN   3
#define IOCTL_VGA_TOGGLE_CURSOR 4
#define IOCTL_VGA_CLEAR_MEM     5
#define IOCTL_VGA_SHOULD_SCROLL 6


//void printk_initialize(void (*putch)(char));
void printk(char* str, ...);

void kernel_panic(const char* message, cpu_registers_t* regs);

#endif
