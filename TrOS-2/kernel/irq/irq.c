#include <TrOS/irq.h>
#include <TrOS/TrOS.h>
//#include <TrOS/hal/VGA.h>


void isr_default_handler(irq_registers_t regs)
{
    printk("Recieved ISR: %d \n", regs.irq_no);
}

void irq_default_handler(irq_registers_t regs)
{
    printk("Recieved IRQ: %d \n", regs.irq_no);
}
