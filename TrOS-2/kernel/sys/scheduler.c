#include <TrOS/scheduler.h>
#include <TrOS/irq.h>
#include <TrOS/hal/pit.h>
#include <TrOS/hal/VGA.h>   //debug purposes for now
#include <TrOS/TrOS.h>      //debug purposes for now

unsigned int ticks;

static void scheduler_irq_callback(cpu_registers_t* regs)
{
    ticks++;
    vga_position_t pos = vga_get_position();
    vga_set_position(67,0);
    printk("Ticks: %d\n", ticks);
    vga_set_position(pos.x, pos.y);

}

void scheduler_initialize(unsigned int frequency)
{
    ticks = 0;
    irq_handler_register(32, &scheduler_irq_callback);

    pit_initialize(frequency);
}
