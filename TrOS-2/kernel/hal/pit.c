#include <TrOS/hal/pit.h>
#include <TrOS/hal/io.h>

#define PIT_8253_CLK 1193180

//NOTE: IRQs will occur as soon as this method is executed
void pit_initialize(unsigned int frequency)
{
    unsigned int divisor = PIT_8253_CLK / frequency;

    // Send the command byte.
    outb(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    // Send the frequency divisor.
    outb(0x40, (unsigned char)(divisor & 0xFF));
    outb(0x40, (unsigned char)((divisor>>8) & 0xFF));
}
