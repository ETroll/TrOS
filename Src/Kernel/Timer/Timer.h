#ifndef _TIMER_H
#define _TIMER_H

//TIMER_TICK specifies the timer tick interval in µsec. Recommended values are 1000 to 100000.
//The resulting interval is from 1 ms to 100 ms. Default configuration is for 10 ms.
#define TIMER_TICK 10000	//uS

void timer_init();
void timer_irq_ack();

#endif
