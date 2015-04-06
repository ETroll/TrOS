#ifndef SRC_KERNEL_IRQ_IRQ_H_
#define SRC_KERNEL_IRQ_IRQ_H_

#define MAX_IRQ 10

//There are 96 different interrupt sources for the RPI
typedef enum
{
	IRQ_ARM_TIMER = 1,
	IRQ_MAILBOX = 2,
	IRQ_UART = 90,
	IRQ_USB = 43,
	IRQ_PENDING1 = 9,
	IRQ_PENDING2 = 10,
	IRQ_DMA1 = 48,
	IRQ_DMA2 = 47,
	IRQ_I2C = 86,
	IRQ_SPI = 87,
	IRQ_TIMER0 = 33,
	IRQ_TIMER1 = 34,
	IRQ_TIMER2 = 35,
	IRQ_TIMER3 = 36
} irq_t;

//TMP stuct before a MM and linked list can be implemented
struct irq_handler
{
	irq_t irq;
	int (*handler) (irq_t irq);
	int open; //TODO: remove open, add next and prev
};

void irq_debug();

void irq_init_data(void);

void irq_global_enable(void);
void irq_global_disable(void);
void irq_global_reset(void);

void irq_enable(irq_t irq);
void irq_disable(irq_t irq);

int irq_handler_register(irq_t irq, int (*handler) (irq_t irq));
struct irq_handler* irq_find_handler(irq_t irq);
void irq_handler_free(irq_t irq);

// Returns the number of IRQs and populates
// the memory with prioritized IRQs
unsigned int irq_waiting(irq_t* irqs);

#endif
