#ifndef _RASPBERRY_H
#define _RASPBERRY_H

#ifdef RPI2
    #define RPI_PERIF_BASE  0x3F000000
#else
    #define RPI_PERIF_BASE  0x20000000
#endif

#define AUX_ENABLES         			(RPI_PERIF_BASE + 0x215004)

//---- GPIO

#define GPIO_BASE           				(RPI_PERIF_BASE + 0x200000)

#define GPIO_FUNCSEL_BASE      	GPIO_BASE
#define GPIO_FUNCSEL_0 			    	GPIO_FUNCSEL_BASE
#define GPIO_FUNCSEL_1      			(GPIO_BASE + 0x04)
#define GPIO_FUNCSEL_2      			(GPIO_BASE + 0x08)
#define GPIO_FUNCSEL_3      			(GPIO_BASE + 0x0C)

#define GPIO_SET_0          				(GPIO_BASE + 0x1C)
#define GPIO_SET_1          				(GPIO_BASE + 0x20)
#define GPIO_SET_2          				(GPIO_BASE + 0x24)
#define GPIO_CLR_0          				(GPIO_BASE + 0x28)
#define GPIO_CLR_1         				(GPIO_BASE + 0x2C)
#define GPIO_CLR_2          				(GPIO_BASE + 0x30)

//GPIO Pin Pull-up/down Enable
#define GPIO_PUD            				(GPIO_BASE + 0x94)
#define GPIO_PUD_CLK0       			(GPIO_BASE + 0x98)
#define GPIO_PUD_CLK1       			(GPIO_BASE + 0x9C)

//---- TIMER
//---- ARM-TIMER
#define ARM_TIMER_BASE      			(RPI_PERIF_BASE + 0xB400)
#define ARM_TIMER_LOAD      			(ARM_TIMER_BASE)
#define ARM_TIMER_VALUE     		(ARM_TIMER_BASE + 0x04)
#define ARM_TIMER_CTRL      			(ARM_TIMER_BASE + 0x08)
#define ARM_TIMER_IRQ_CLEAR 		(ARM_TIMER_BASE + 0x0C)
#define ARM_TIMER_RAW_IRQ   		(ARM_TIMER_BASE + 0x10)
#define ARM_TIMER_MASKD_IRQ		(ARM_TIMER_BASE + 0x14)
#define ARM_TIMER_RELOAD    		(ARM_TIMER_BASE + 0x18)
#define ARM_TIMER_PRE_DIV   		(ARM_TIMER_BASE + 0x1C)
#define ARM_TIMER_COUNTER   		(ARM_TIMER_BASE + 0x20)

//---- SYSTEM TIMER
#define SYSTEM_TIMER_BASE   		(RPI_PERIF_BASE + 0x3000)
#define SYSTEM_TIMER_CLO    		(SYSTEM_TIMER_BASE + 0x04)

//---- IRQ
#define IRQ_BASE            					(RPI_PERIF_BASE + 0xB000)	//ARMCTRL_BASE
#define IRQ_BASIC_PEND      			(IRQ_BASE + 0x200)		//ARMCTRL_IC_BASE
#define IRQ_GPU_PEND1       			(IRQ_BASE + 0x204)
#define IRQ_GPU_PEND2       			(IRQ_BASE + 0x208)
#define IRQ_FIQ_CONTROL     			(IRQ_BASE + 0x20C)
#define IRQ_GPU_ENABLE      			(IRQ_BASE + 0x210)
#define IRQ_GPU_ENABLE2     			(IRQ_BASE + 0x214)
#define IRQ_BASIC_ENABLE    			(IRQ_BASE + 0x218)
#define IRQ_GPU_DISABLE1    			(IRQ_BASE + 0x21C)
#define IRQ_GPU_DISABLE2    			(IRQ_BASE + 0x220)
#define IRQ_BASIC_DISABLE   			(IRQ_BASE + 0x224)

//---- UART
#define UART0_BASE          				(RPI_PERIF_BASE + 0x201000)
#define UART0_IO_REG        			(UART0_BASE + 0x00)
#define UART0_FLAG_REG      			(UART0_BASE + 0x18)
#define UART0_INT_BAUD      			(UART0_BASE + 0x24)
#define UART0_FRAC_BAUD     		(UART0_BASE + 0x28)
//Line Control register
#define UART0_LINE_CTRL_REG 		(UART0_BASE + 0x2c)
//Control register
#define UART0_CTRL_REG      			(UART0_BASE + 0x30)
//Interupt Mask Set Clear Register
#define UART0_INTR_MSC      			(UART0_BASE + 0x38)
//Masked Interupt Status Register
#define UART0_INTR_MIS      			(UART0_BASE + 0x40)
//Interupt Clear Register
#define UART0_INTR_ICR  				(UART0_BASE + 0x44)
#define UART_READY  0

//---- SPI
#define SPI0_BASE           				(RPI_PERIF_BASE + 0x204000)
#define SPI0_CTRL_STATUS    			(SPI0_BASE)
#define SPI0_FIFO           					(SPI0_BASE + 0x04)
#define SPI0_CLK_DIVIDER    			(SPI0_BASE + 0x08)
#define SPI0_DATA_LENGTH    			(SPI0_BASE + 0x0C)
#define SPI0_LTOH           				(SPI0_BASE + 0x10)
#define SPI0_DC             					(SPI0_BASE + 0x14)



#endif
