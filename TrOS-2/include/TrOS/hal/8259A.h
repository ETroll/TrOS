// 8259A.h
// Hardware support for the 8259A PIC

#ifndef INCLUDE_TROS_8259A_H
#define INCLUDE_TROS_8259A_H

#define PIC1_CREG   0x20
#define PIC2_CREG   0xA0
#define PIC1_DREG   0x21
#define PIC2_DREG   0xA1

void init_8259A();
void eoi_8259A();

#endif
