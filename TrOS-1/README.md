### TrOS-1

This was my first ever attempt at writing a OS.

This is a very simple and primitive OS for RaspberryPi2. It has multitasking and preemptive scheduling and some simple driver "structure" added. There is no memory protection or memory allocation, all is done via stack. After the kernel is initialized it startes a userland application that basically is a echo application on the UART0 and has 1 or 2 commands that does anything sound or tests things.
