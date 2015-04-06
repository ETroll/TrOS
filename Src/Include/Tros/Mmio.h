#ifndef SRC_INCLUDE_KERNEL_MMIO_H_
#define SRC_INCLUDE_KERNEL_MMIO_H_

extern void NOP(void);
extern void dummy(unsigned int);
extern void mmio_write(unsigned int addr, unsigned int data);
extern unsigned int mmio_read(unsigned int addr);

#endif /* SRC_INCLUDE_KERNEL_MMIO_H_ */
