#include <tros/scheduler.h>
#include <tros/irq.h>
#include <tros/hal/pit.h>
#include <tros/hal/VGA.h>   //debug purposes for now
#include <tros/tros.h>      //debug purposes for now

static unsigned int ticks;

static vga_char_attrib_t menu_color = {
    .bg = VGA_WHITE,
    .font = VGA_BLACK
};

static void scheduler_irq_callback(cpu_registers_t* regs)
{
    ticks++;
    //vga_position_t pos = vga_get_position();
    //vga_set_position(69,23);
    //vga_set_color(&menu_color);
    //printk("Ticks: %d\n", ticks);
    //vga_set_position(pos.x, pos.y);
    irq_eoi(0);
}

void scheduler_initialize(unsigned int frequency)
{
    ticks = 0;
    irq_register_handler(32, &scheduler_irq_callback);

    pit_initialize(frequency);
}

void scheduler_sleep(unsigned int wait)
{
    wait = wait + ticks;
    while(wait > ticks)
    {
        __asm("nop;");
    }
}
