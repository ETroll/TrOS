#include <tros/timer.h>
#include <tros/irq.h>
#include <tros/hal/pit.h>
#include <tros/tros.h>      //debug purposes for now
#include <tros/sched/scheduler.h>

static unsigned int ticks;

// static vga_char_attrib_t menu_color = {
//     .bg = VGA_WHITE,
//     .font = VGA_BLACK
// };

static void timer_irq_callback(cpu_registers_t* regs)
{
    ticks++;
    // printk("# EIP %x\n", regs->eip);
    //BOCHS_DEBUG
    scheduler_reschedule();
    irq_eoi(0);
}

void timer_initialize(unsigned int frequency)
{
    ticks = 0;
    irq_register_handler(32, &timer_irq_callback);

    pit_initialize(frequency);
}

void timer_sleep(unsigned int wait)
{
    wait = wait + ticks;
    while(wait > ticks)
    {
        __asm("nop;");
    }
}
