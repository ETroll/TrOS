#include <Tros/Kernel.h>
#include <Tros/Drivers.h>
#include <Tros/Utils.h>

volatile struct serial_driver* __pk_serial = 0;

void _init_printk()
{
	__pk_serial = 0;
}

void printk(char* str, ...)
{
	register int* arguments = (int *)(&str);

	//uart_hexvalue(__pk_serial);
	if(__pk_serial == 0)
	{
		struct device_driver* driver = drivers_find_device("uart0");

		if(driver != NULL && driver->type == DRV_SERIAL)
		{
			__pk_serial = (struct serial_driver*)driver->driver;
			__pk_serial->open();
		}
	}
	__pk_serial->write("Kernel: ", 8);

	while (*str)
	{
		if (*str != '%')
		{
			__pk_serial->writechar(*str);
		}
		else
		{
			str++;
			arguments++;

			switch(*str)
			{
				case 'd':
				{
					//MINI itoa (Maybe break it out?)
					int arg = *arguments;
					char buffer[10];
					int digit = 0;

					if(arg < 0){
						__pk_serial->writechar('-');
						arg *= -1;
					}

					if(arg == 0)
					{
						__pk_serial->writechar('0');
					}
					else {
						while (arg > 0 && digit <= 9) {
							buffer[digit++] =  0x30 + (arg % 10);
							arg = arg / 10;
						}

						for(int i = digit-1; i>=0; i--)
						{
							__pk_serial->writechar(buffer[i]);
						}
					}
				}
				break;
				case 'k':
				{
					//special case for kernel mode
					cpu_mode_t mode = (cpu_mode_t)(*arguments);
					switch(mode)
					{
						case CPU_ABORT:
							__pk_serial->write("ABT", 3);
						break;
						case CPU_SUPERVISOR:
							__pk_serial->write("SVR", 3);
						break;
						case CPU_USER:
							__pk_serial->write("USR", 3);
						break;
						case CPU_IRQ:
							__pk_serial->write("IRQ", 3);
						break;
						case CPU_FIQ:
							__pk_serial->write("FIQ", 3);
						break;
						case CPU_SYSTEM:
							__pk_serial->write("SYS", 3);
						break;
						default:
							__pk_serial->write("UND", 3);
						break;
					}
				}
				break;
				case 'x':
				case 'X':
				{
					int arg = *arguments;
					unsigned int nibble;

					__pk_serial->writechar('0');
					__pk_serial->writechar('x');
					for(int i = 32; i >= 0; i-=4)
					{
						nibble = (arg >> i) & 0xF;
						if(nibble > 9)
						{
							nibble += 0x37;
						}
						else
						{
							nibble += 0x30;
						}
						__pk_serial->writechar(nibble);
					}
				}
				/* no break */
				default:
				break;
			}
		}
		str++;
	}
	//__pk_serial->close(); //no need to close it for every write now.
}
