#include <tros/hal/VGA.h>

void printk(char* str, ...)
{
	register int* arguments = (int *)(&str);

	while (*str)
	{
		if (*str != '%')
		{
			vga_putch(*str);
		}
		else
		{
			str++;
			arguments++;

			switch(*str)
			{
				case 's':
				{
					const char* str = (const char*)*arguments;
					vga_puts(str);
				}
				break;

				case 'c':
				{
					char arg = *arguments;
					vga_putch(arg);
				}
				break;

				case 'i':
				case 'd':
				{
					//MINI itoa (Maybe break it out?)
					int arg = *arguments;
					char buffer[10];
					int digit = 0;

					if(arg < 0)
					{
						vga_putch('-');
						arg *= -1;
					}

					if(arg == 0)
					{
						vga_putch('0');
					}
					else
					{
						while (arg > 0 && digit <= 9)
						{
							buffer[digit++] =  0x30 + (arg % 10);
							arg = arg / 10;
						}
						for(int i = digit-1; i>=0; i--)
						{
							vga_putch(buffer[i]);
						}
					}
				}
				break;

				case 'x':
				case 'X':
				{
					int arg = *arguments;
					unsigned int nibble;

					vga_putch('0');
					vga_putch('x');
					for(int i = 28; i >= 0; i-=4)
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
						vga_putch(nibble);
					}
				}
				/* no break */
				default:
				break;
			}
		}
		str++;
	}
}
