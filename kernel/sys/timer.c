#include <tros/timer.h>
#include <tros/irq.h>
#include <tros/hal/pit.h>
#include <tros/tros.h>
#include <tros/memory.h>

typedef struct timer_thread_sleep
{
    thread_t* thread;
    uint32_t ticks;
    struct timer_thread_sleep* next;
} timer_thread_sleep_t;

static uint32_t ticks = 0;
static timer_thread_sleep_t* sleep_delta = 0;

static void timer_irq_callback(cpu_registers_t* regs)
{
    ticks++;

    if(sleep_delta != 0)
    {
        if(sleep_delta->ticks > 0)
        {
            sleep_delta->ticks--;
        }
        else
        {
            while(sleep_delta->ticks == 0)
            {
                thread_setState(sleep_delta->thread, THREAD_RUNNING);
                timer_thread_sleep_t* prev = sleep_delta;
                sleep_delta = prev->next;
                kfree(prev);
            }
        }
    }

    scheduler_reschedule();
    irq_eoi(0);
}

void timer_initialize(unsigned int frequency)
{
    irq_register_handler(32, &timer_irq_callback);
    pit_initialize(frequency);
}

void timer_sleep(thread_t* thread, unsigned int ticks)
{
    //http://wiki.osdev.org/Blocking_Process
    timer_thread_sleep_t* node = (timer_thread_sleep_t*)kmalloc(sizeof(timer_thread_sleep_t));
    node->thread = thread;
    node->ticks = ticks;
    node->next = 0;

    if(sleep_delta == 0)
    {
        sleep_delta = node;
    }
    else
    {
        timer_thread_sleep_t* next = sleep_delta;
        timer_thread_sleep_t* prev = 0;

        uint32_t sum = 0;
        while(next && node->ticks > (sum + next->ticks))
        {
            sum += next->ticks;
            prev = next;
            next = next->next;
        }

        if(node->ticks <= sleep_delta->ticks)
        {
            sleep_delta = node;
        }

        node->ticks -= sum;
        node->next = next;
        if(prev)
        {
            prev->next = node;
        }

        while(next && next->ticks > 0)
        {
            next->ticks -= node->ticks;
            next = next->next;
        }
    }

    thread_setState(thread, THREAD_SLEEPING);
    scheduler_reschedule();
}
