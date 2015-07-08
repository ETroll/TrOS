#include <TrOS/irq.h>
#include <TrOS/TrOS.h>
//#include <TrOS/hal/VGA.h>

void irq_default_handler(irq_registers_t regs)
{
    printk("Recieved IRQ: %d \n", regs.irq_no);
}
