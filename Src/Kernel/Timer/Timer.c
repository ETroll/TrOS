#include "Timer.h"
#include <Tros/Mmio.h>
#include <Tros/Arch/Raspberry.h>


// Section 14 of the BCM2835 Peripherals PDF
void timer_init()
{
    mmio_write(ARM_TIMER_LOAD, 0x100);
    mmio_write(ARM_TIMER_CTRL, (1 << 1) | (1 << 7) | (1 << 5) | (2 << 2));
}
void timer_irq_ack()
{
    mmio_write(ARM_TIMER_IRQ_CLEAR,0);
}
