#ifndef INCLUDE_KERNEL_H
#define INCLUDE_KERNEL_H

#define GenerateInterrupt(arg) __asm__("int %0\n" : : "N"((arg)) : "cc", "memory")

//void printk_initialize(void (*putch)(char));
void printk(char* str, ...);

#endif
