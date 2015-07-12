#include <TrOS/hal/idt.h>
#include <TrOS/hal/gdt.h>
#include <TrOS/hal/pic.h>
#include <TrOS/TrOS.h>
#include <TrOS/irq.h>

static irq_handler __irq_handlers[MAX_IRQ];

static const char* irq_names[32] = {
	"Division by zero",
	"Debug",
	"Non-maskable interrupt",
	"Breakpoint",
	"Detected overflow",
	"Out-of-bounds",
	"Invalid opcode",
	"No coprocessor",
	"Double fault",
	"Coprocessor segment overrun",
	"Bad TSS",
	"Segment not present",
	"Stack fault",
	"General protection fault",
	"Page fault",
	"Unknown interrupt",
	"Coprocessor fault",
	"Alignment check",
	"Machine check",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

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

void irq_default_handler(cpu_registers_t regs)
{
    printk("Recieved IRQ: %d \n", regs.irq_no);
    if(regs.irq_no > 31)
    {
        pic_eoi(regs.irq_no);
    }

    if (__irq_handlers[regs.irq_no] != 0)
    {
        irq_handler handler = __irq_handlers[regs.irq_no];
        handler(regs);
    }
    else if(regs.irq_no < 32)
    {
		// If we have a CPU exception and it is not handled
		// we are in touble.. Lets panic. (We did not bring a towel..)
		kernel_panic(irq_names[regs.irq_no], &regs);
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
