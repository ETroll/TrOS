#include <Tros/Utils.h>
#include <Tros/Mmio.h>
#include <Tros/Kernel.h>
#include <Tros/IRQ.h>
#include <Tros/Arch/Raspberry.h>

/*
 * Going to limit to 1 IRQ handler per IRQ for now.
 */

extern void __irq_global_enable(void);
extern void __irq_global_disable(void);

static struct irq_handler __irq_handlers[MAX_IRQ];
static int __irq_counter = 0;

void irq_init_data(void)
{
	for(int i = 0; i<MAX_IRQ; i++)
	{
		__irq_handlers[i].handler = NULL;
		__irq_handlers[i].irq = -1;
		__irq_handlers[i].open = 0;
	}
	__irq_counter = 0;
}

void irq_global_enable(void)
{	//Remove?
	__irq_global_enable();
}
void irq_global_disable(void)
{	//Remove?
	__irq_global_disable();
}

void irq_global_reset(void)
{	//Remove?
	__irq_global_disable();
	__irq_global_enable();
}

void irq_enable(irq_t irq)
{
	int _irq = irq -1;
	int offset = _irq / 32;
	unsigned int irq_enable_reg = IRQ_BASIC_ENABLE;

	if(offset > 0)
	{
		if(offset == 1)
		{
			irq_enable_reg = IRQ_GPU_ENABLE;
		}
		else
		{
			irq_enable_reg = IRQ_GPU_ENABLE2;
		}
	}
	//printk("Enabling IRQ: %d - Offset: %d - Data: %x\n", _irq, offset, (1 << (_irq-(32*offset))));
	unsigned int reg = mmio_read(irq_enable_reg);
	reg |= (1 << (_irq-(32*offset)));
    mmio_write(irq_enable_reg, reg);
}

void irq_disable(irq_t irq)
{
	int _irq = irq -1;
	int offset = _irq / 32;
	unsigned int irq_disable_reg = IRQ_BASIC_DISABLE;

	if(offset > 0)
	{
		if(offset == 1)
		{
			irq_disable_reg = IRQ_GPU_DISABLE1;
		}
		else
		{
			irq_disable_reg = IRQ_GPU_DISABLE2;
		}
	}
	//printk("Disabling IRQ: %d - Offset: %d - Data: ~%x\n", _irq, offset, (1 << (_irq-(32*offset))));
	unsigned int reg = mmio_read(irq_disable_reg);
	reg &= ~(1 <<  (_irq-(32*offset)));
	mmio_write(irq_disable_reg, reg);
}

void irq_handler_free(irq_t irq)
{
	for(int i = 0; i<MAX_IRQ; i++)
	{
		if(__irq_handlers[i].irq == irq)
		{
			__irq_handlers[i].handler = NULL;
			__irq_handlers[i].irq = 0;
			__irq_handlers[i].open = 0;

			irq_disable(irq);
			break;
		}
	}
}

int irq_handler_register(irq_t irq, int (*handler) (irq_t irq))
{
	for(int i = 0; i<MAX_IRQ; i++)
	{
		if(__irq_handlers[i].irq == irq)
		{
			return -1;
		}
	}

	__irq_handlers[__irq_counter].handler = handler;
	__irq_handlers[__irq_counter].irq = irq;
	__irq_handlers[__irq_counter].open = 1;

	irq_enable(irq);

	return __irq_counter++;
}


struct irq_handler* irq_find_handler(irq_t irq)
{
	for(int i = 0; i<MAX_IRQ; i++)
	{
		if(__irq_handlers[i].irq == irq)
		{
			//printk("Handler (%d) for IRQ %d at %x\n", i, __irq_handlers[i].irq, __irq_handlers[i].handler);
			return &__irq_handlers[i];
		}
	}
	return NULL;
}

unsigned int irq_waiting2(irq_t* irqs)
{
	unsigned int irq_pending = mmio_read(IRQ_BASIC_PEND);
	unsigned int num = 0;

	for(int i = 0; i<32; i++)
	{
		unsigned int check = (1 << i);
		if((irq_pending & check))
		{
			irqs[num++] = i+1;
			printk("Checking %x against %x Found! (%d)\n", (1 << i) , irq_pending, i+1);
		}
	}
	return num;
}

unsigned int irq_waiting(irq_t* irqs)
{
	unsigned int irq_basic = mmio_read(IRQ_BASIC_PEND);
	unsigned int num = 0;

	//Check basic / ARM
	if((irq_basic & 0x000000FF))
	{
		for(int i = 0; i<8; i++)
		{
			if((irq_basic & (1 << i)))
			{
				irqs[num++] = 1 + i;
			}
		}
	}

	//Check GPU 1;
	if((irq_basic & 0x00007D00))
	{
		unsigned int irq_gpu1 = mmio_read(IRQ_GPU_PEND1);
		for(int i = 0; i<32; i++)
		{
			if((irq_gpu1 & (1 << i)))
			{
				irqs[num++] = 33 + i;
			}
		}
	}

	//Check GPU 2;
	if((irq_basic & 0x000F8000))
	{
		unsigned int irq_gpu2 = mmio_read(IRQ_GPU_PEND2);
		for(int i = 0; i<32; i++)
		{
			if((irq_gpu2 & (1 << i)))
			{
				irqs[num++] = 65 + i;
			}
		}
	}

	return num;
}


void irq_debug()
{
	printk("\n\n---- UART DEBUG: ----\n");
	printk("IRQ_BASIC_PEND: %x\n", mmio_read(IRQ_BASIC_PEND));
	printk("IRQ_GPU_PEND1: %x\n", mmio_read(IRQ_GPU_PEND1));
	printk("IRQ_GPU_PEND2: %x\n", mmio_read(IRQ_GPU_PEND2));
	printk("----\n");
	printk("IRQ_BASIC_ENABLE: %x\n", mmio_read(IRQ_BASIC_ENABLE));
	printk("IRQ_GPU_ENABLE: %x\n", mmio_read(IRQ_GPU_ENABLE));
	printk("IRQ_GPU_ENABLE2: %x\n", mmio_read(IRQ_GPU_ENABLE2));
	printk("----\n");
	printk("IRQ_BASIC_DISABLE: %x\n", mmio_read(IRQ_BASIC_DISABLE));
	printk("IRQ_GPU_DISABLE1: %x\n", mmio_read(IRQ_GPU_DISABLE1));
	printk("IRQ_GPU_DISABLE2: %x\n", mmio_read(IRQ_GPU_DISABLE2));
}
