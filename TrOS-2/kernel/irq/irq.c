#include <TrOS/hal/idt.h>
#include <TrOS/hal/gdt.h>
#include <TrOS/hal/pic.h>
#include <TrOS/TrOS.h>
#include <TrOS/irq.h>

static irq_handler __irq_handlers[MAX_IRQ];

void irq_initialize(void)
{
    for(int i = 0; i<MAX_IRQ; i++)
	{
		__irq_handlers[i] = 0;
	}

    gdt_initialize();
    idt_initialize();
    pic_initialize();
}

void isr_default_handler(irq_registers_t regs)
{
    printk("Recieved ISR: %d Error: %d\n", regs.irq_no, regs.err_code);

    if (__irq_handlers[regs.irq_no] != 0)
    {
        printk("A handler is registeres\n");
        irq_handler handler = __irq_handlers[regs.irq_no];
        handler(regs);
    }
    if(regs.irq_no == 13)
    {
        //double fault... something is very very wrong
        __asm("hlt");
    }
}

void irq_default_handler(irq_registers_t regs)
{
    printk("Recieved IRQ: %d \n", regs.irq_no);
    pic_eoi(regs.irq_no);

    if (__irq_handlers[regs.irq_no] != 0)
    {
        irq_handler handler = __irq_handlers[regs.irq_no];
        handler(regs);
    }
}

int irq_handler_register(unsigned int irq, irq_handler handler)
{
    if(__irq_handlers[irq] != 0)
    {
    	return -1;
    }
    else
    {
        __irq_handlers[irq] = handler;
        return irq;
    }
}
