/////
//
// Platform Baseboard for ARM1176JZF-S. Used for QEMU debugging
//
// Doc:
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0425f/index.html
//
/////


#ifndef _VERSATILEPB_H
#define _VERSATILEPB_H

#define ARM_CTRL_REG_BASE   0x10000000
#define ARM_SYS_ID          (ARM_CTRL_REG_BASE)

//---- UART

#define UART0_BASE          0x101F1000
#define UART1_BASE          0x101F2000
#define UART2_BASE          0x101F3000

#define UART0_IO_REG        (UART0_BASE + 0x00)
#define UART0_FLAG_REG      (UART0_BASE + 0x18)

#define UART_READY  0


//---- TIMERS

#define TIMER0              0x101E2000
#define TIMER_VALUE         0x04
#define TIMER_CONTROL       0x08
#define TIMER_INTCLR        0x0C
#define TIMER_MIS           0x14

#define TIMER_EN            0x80
#define TIMER_PERIODIC      0x40
#define TIMER_INTEN         0x20
#define TIMER_32BIT         0x02
#define TIMER_ONESHOT       0x01

#endif
