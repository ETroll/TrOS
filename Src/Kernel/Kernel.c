
#include "Timer/Timer.h"
#include "Scheduling/Task.h"

#include <Tros/Kernel.h>
#include <Tros/Drivers.h>
#include <Tros/IRQ.h>
#include <Tros/Syscalls.h>
#include <Tros/Gpio.h>
#include <Tros/Utils.h>


extern void _init_printk(void);
extern void app_init(void);

void kmain()
{
	//1: Set up arch specific stuff. (Tty, Timer, IRQ)
	//2: Start Init task. (This will load drivers, and start core processes)
	//3: Exit Init and start Terminal

    uint pid_count = 0;
    struct task* current_task = NULL;
    uint kernel_status_flags = 0;
    uint led_timer_counter = 0;

    init_device_driver_struct();	//temp while no MM is active
    irq_init_data();					//temp while no MM is active

    uart_init_driver();
    _init_printk();		//Normally one would set up some sort of
    // terminal and connect the printk to that instead.

    timer_init();
    irq_enable(IRQ_ARM_TIMER);
    printk("[OK] Timer initialized\n");

    irq_global_enable();
    printk("[OK] IRQ globaly enabled\n");

    //Set up LED
    gpio_enable_pin(47, GPIO_OUT);
    gpio_clear_pin(47);


    //Lets set up tasking...
    unsigned int stacks[TASK_LIMIT][STACK_SIZE];
    struct ringbuffer pipes[PIPE_LIMIT];
    struct task tasks[TASK_LIMIT];

    for (int i = 0; i < TASK_LIMIT; i++)
    {
        tasks[i].PID = -1;
        tasks[i].status = TASK_DEAD;
    }

    for(int i = 0; i < PIPE_LIMIT; i++)
    {
        pipes[i].start = pipes[i].end = 0;
    }

    int next_stackid = task_nextfree(tasks, TASK_LIMIT);

    tasks[next_stackid].PID = pid_count++;
    tasks[next_stackid].status = TASK_RUNNING;
    tasks[next_stackid].task_id = next_stackid;
    tasks[next_stackid].stack = task_stack_init(stacks[next_stackid], &app_init);
    tasks[next_stackid].next = &tasks[next_stackid];
    tasks[next_stackid].prev = &tasks[next_stackid];

    current_task = &tasks[next_stackid];

    printk("[OK] Scheduler initialized\n");

    while(TRUE)
    {
    	unsigned int num_irqs = 0;
    	irq_t interrupts[MAX_IRQ];

        current_task->stack = activate(current_task->stack);

        // tasks[current_task][2+7]) = R7 set in userland in the syscall
        // before calling the interrupt and ending up in the interrupt routine
        // leading here. (Since we activated the task that returns here using
        // the interrupt right abve this line of code)
        switch(current_task->stack[2+7])
        {
			case 0x0: //YIELD
				//printk("YIELD!\n");
			break;
            case 0x1: //FORK
            {
            		//printk("FORK!\n");
                next_stackid = task_nextfree(tasks, TASK_LIMIT);

                if(next_stackid < 0)
                {
                    current_task->stack[2+0] = next_stackid; //Set R0 to -1
                    printk("#### ERRORR #### NO MORE FREE TASK IDs\n");
                }
                else
                {
                    unsigned int used = stacks[current_task->task_id] + STACK_SIZE - current_task->stack;

                    tasks[next_stackid].PID = pid_count++;
                    tasks[next_stackid].status = TASK_RUNNING;
                    tasks[next_stackid].task_id = next_stackid;
                    tasks[next_stackid].stack = task_stack_copy(current_task->stack,
                        stacks[next_stackid],
                        used);

                    //Add to task list
                    tasks[next_stackid].parent = current_task;
                    tasks[next_stackid].next = current_task->next;
                    tasks[next_stackid].prev = current_task;
                    tasks[next_stackid].next->prev = &tasks[next_stackid];
                    current_task->next = &tasks[next_stackid];

                    //set return values
                    current_task->stack[2+0] = pid_count;
                    tasks[next_stackid].stack[2+0] = 0;
                }
            }
            break;
            case 0x2: //EXIT
            {
                printk("PID %d wants to exit with code: %d\n",
                    current_task->PID,
                    current_task->stack[2+0]);
                current_task->status = TASK_DEAD;
                current_task->PID = -1;
                current_task->stack = NULL;

                //remove from list
                current_task->prev->next = current_task->next;
                current_task->next->prev = current_task->prev;

                //Since we increment current task later, then go back a step
                current_task = current_task->prev;

            }
            break;
            case 0x3: //GETPID
            {
            		printk("%k GETPID!\n", get_krn_mode());
                current_task->stack[2+0] = current_task->PID;
            }
            break;
            case 0x4: //WRITE
            {
            		//printk("PID: %d WRITE(%d)\n",  current_task->PID);

            		uint status = current_task->status;
				uint device_id = current_task->stack[2+0];
				const char* buffer = (const char*)current_task->stack[2+1];
				uint size =  current_task->stack[2+2];

				struct device_driver* device = drivers_find_device_id(device_id);
				if(device != NULL)
				{
					if(device->driver != NULL)
					{
						current_task->status = TASK_WAITING_WRITE;
						struct serial_driver* tmp = (struct serial_driver*)device->driver;
						current_task->stack[2+0] = tmp->write(buffer, size);
						current_task->status = status;
					}
				}
            }
            break;
            case 0x5: //READ
            {
            		//printk("PID: %d READ(%d)\n", current_task->PID);

            		uint status = current_task->status;
            		uint device_id = current_task->stack[2+0];
				char* buffer = (char*)current_task->stack[2+1];
				uint size =  current_task->stack[2+2];

				struct device_driver* device = drivers_find_device_id(device_id);
				if(device != NULL)
				{
					if(device->driver != NULL)
					{
						current_task->status = TASK_WAITING_READ;
						struct serial_driver* tmp = (struct serial_driver*)device->driver;
						current_task->stack[2+0] = tmp->read(buffer, size);
						current_task->status = status;
					}
				}
            }
            break;
            case 0x6: //OPEN
			{
				//printk("PID: %d OPEN! (Sesame?)\n", current_task->PID);

				char* name = (char*)current_task->stack[2+0];
				current_task->stack[2+0] = -1;

				struct device_driver* device = drivers_find_device(name);
				if(device != NULL)
				{
					if(device->driver != NULL)
					{
						struct serial_driver* tmp = (struct serial_driver*)device->driver;
						if(tmp->open())
						{
							current_task->stack[2+0] = device->device_id;
						}
					}
				}
			}
			break;
			case 0x7: //CLOSE
			{
				//printk("PID: %d CLOSE!\n", current_task->PID);

				uint device_id = (uint)current_task->stack[2+0];
				current_task->stack[2+0] = -1;
				struct device_driver* device = drivers_find_device_id(device_id);
				if(device != NULL)
				{
					if(device->driver != NULL)
					{
						struct serial_driver* tmp = (struct serial_driver*)device->driver;
						current_task->stack[2+0] = tmp->close();
					}
				}
			}
			break;
			case 0x8: //PEEK
			{
				uint device_id = (uint)current_task->stack[2+0];
				current_task->stack[2+0] = -1;
				struct device_driver* device = drivers_find_device_id(device_id);
				if(device != NULL)
				{
					if(device->driver != NULL)
					{
						struct serial_driver* tmp = (struct serial_driver*)device->driver;
						current_task->stack[2+0] = tmp->peek();
					}
				}
			}
			break;
            case 0x100: //IRQ
            {
            		num_irqs = irq_waiting(interrupts);
            		if(num_irqs > 0)
            		{
            			for(int i = 0; i<num_irqs; i++)
					{
						if(interrupts[i] == IRQ_ARM_TIMER)
						{
							if(led_timer_counter > 5)
							{
								if(kernel_status_flags & 0x1)
								{
									gpio_clear_pin(47);
									kernel_status_flags &= ~0x1;
								}
								else
								{
									gpio_set_pin(47);
									kernel_status_flags |= 0x1;
								}
								led_timer_counter = 0;
							}
							led_timer_counter++;
							timer_irq_ack();
						}
						else
						{
							struct irq_handler* handler = irq_find_handler(interrupts[i]);
							//printk("IRQ %d - Handler found: %d\n", interrupts[i], handler == NULL ? 0 : 1 );

							if(handler != NULL)
							{
								handler->handler(interrupts[i]);
							}
							else
							{
								irq_disable(interrupts[i]);
							}
						}
					}
            		}
            		else
            		{
            			printk("Irq not found\n");
            			irq_debug();
            		}
            }
            	break;
            default:
            {
            		printk("Unknown syscall\n");
              	printk("Hex: %x\r\nDec: %d\n",
               	current_task->stack[2+7],
             	current_task->stack[2+7]);
            }
            break;
        }

        //Handleded syscall from the task started above. Now time to move to next
        do
        {
            //HANDLE SCENARIO WHERE THERE ARE NO RUNNING TASKS
            current_task = current_task->next;
        }
        while(current_task->status != TASK_RUNNING);


#ifdef DEBUG
        unsigned int used = stacks[current_task->task_id] + STACK_SIZE - current_task->stack;

        printk("Stack trace:\n");
        for(int i = 0; i<used-1; i++)
        {
            printk("[%d] - %x: %x\n", i, &current_task->stack[i], current_task->stack[i]);
        }
#endif
        //printk("Tasking\n");
        //timer_irq_ack();
    }

    while(TRUE);
}

void crashdump()
{
	printk("Crash!");
	while(TRUE);
}

cpu_mode_t get_krn_mode()
{
	unsigned int tmp = 0;
	__asm("mrs %[res], CPSR" : [res] "=r" (tmp));
	return (cpu_mode_t) (tmp & 0xF);
}

