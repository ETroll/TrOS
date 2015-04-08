#ifndef SRC_INCLUDE_KERNEL_KERNEL_H_
#define SRC_INCLUDE_KERNEL_KERNEL_H_

typedef enum
{
	CPU_SUPERVISOR = 0x3,
	CPU_USER = 0x0,
	CPU_IRQ = 0x2,
	CPU_FIQ = 0x1,
	CPU_SYSTEM = 0xf,
	CPU_ABORT = 0x7,
	CPU_UNDEFINED = 0xb
} cpu_mode_t;

void printk(char* str, ...);
cpu_mode_t get_krn_mode(void);

void* kmalloc(unsigned int size);

#endif
