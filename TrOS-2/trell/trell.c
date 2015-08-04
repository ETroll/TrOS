// Trell - TrOS default shell

#include <trell/trell.h>
#include <tros/tros.h>
#include <tros/driver.h>
#include <tros/hal/VGA.h>
#include <string.h>
#include <stdio.h>
#include <keyboard.h>


#define TRELL_RCOLUMN_SIZE 11
#define TRELL_CONSOLE_SIZE VGA_COLS - TRELL_RCOLUMN_SIZE
#define TRELL_CONSOLE_LINES 23

#define TRELL_MAX_HISTORY 400

extern void (*__putch)(char c);
extern void (*__puts)(const char* str);

static char __trell_history[TRELL_MAX_HISTORY][TRELL_CONSOLE_SIZE];
static unsigned int __trell_next_inputpos;
static unsigned int __trell_current_viewpos;

static void trell_draw_ui();
static void trell_cmd();
void trell_clear();

void trell_initialize()
{
    __putch = trell_putch;
    __puts = trell_puts;
    trell_clear();

    printk("trell_history at %x\n", &__trell_history);

    trell_cmd();
}

void trell_clear()
{
    __trell_next_inputpos = 0;
    __trell_current_viewpos = 0;
    //redraw
}

void trell_putch(char c)
{
    vga_putch(c);
}
void trell_puts(const char* str)
{
    vga_puts(str);
}

void kernel_run_command(char* cmd)
{
	char* argv[5];	//NOTE: Hardcoded a maximum of 5 params for now
	unsigned int argc = 1;

	argv[0] = cmd;
	while(*cmd++ != '\0' && argc < 5)
	{
		if(*cmd == ' ')
		{
			*cmd = '\0';
			argv[argc++] = ++cmd;
		}
	}
	// printk("Got command %s with %d parameters\n", argv[0], argc-1);
	// for(int i=1; i<argc; i++)
	// {
	// 	printk("parameter %d: %s\n", i, argv[i]);
	// }

	if(strcmp(argv[0], "clr") == 0)
	{
        trell_clear();
	}
	else if(strcmp(argv[0], "help") == 0)
	{
		printk("TrOS-2 Help:\n");
		printk("Commands:\n");
		printk(" - help: Displays this help message\n");
		printk(" - cls:  Clears the display\n");
	}
	else if(strcmp(argv[0], "rs") == 0)
	{
		if(argc > 1)
		{
			driver_block_t* fdd = (driver_block_t*)driver_find_device("fdd")->driver;
			if(fdd != 0)
			{
				fdd->open();

				//Bit of a hack for now, since I hardcoded DMA to 0x1000
				unsigned char* buffer = (unsigned char*)0x1000;

				int sect = atoi(argv[1]);
				//printk("Reading sector: %d\n", sect);
				int num = fdd->read(buffer, sect);

				for(int i = 0; i<10; i++)
				{
					printk("%x: %x\n", &buffer[i], buffer[i]);
				}
				printk("\n");
				//printk("\n\n %d sectors read\n", num);

				fdd->close();
			}
			else
			{
				printk("ERROR: Could not find a floppy disk drive\n");
			}
		}
		else
		{
			printk("Usage: rs <sect>, where <sect> is a integer\n");
		}
	}
	else
	{
		printk("Unknown command\n");
	}
}

static void trell_cmd()
{
	char cmd_buffer[100];
	int buffer_loc = 0;
	int next_command = 0;
	int key;
	driver_hid_t* kbd = (driver_hid_t*)driver_find_device("kbd")->driver;
	kbd->open();
	while(1)
	{
		buffer_loc = 0;
		next_command = 0;

		printk("> ");

		while(!next_command)
		{
			key = 0;
			if(kbd->read(&key,1))
			{
				if(key != KEY_LCTRL
					&& key != KEY_RCTRL
					&& key != KEY_RALT
					&& key != KEY_LALT
					&& key != KEY_RSHIFT
					&& key != KEY_LSHIFT
					&& key != KEY_CAPSLOCK)
				{
					if(key == KEY_RETURN)
					{
						cmd_buffer[buffer_loc] = '\0';
						printk("\n");
						kernel_run_command(cmd_buffer);
						next_command = 1;
					}
                    else if(key == KEY_PAGEUP)
					{
                        printk("PGUP\n");
                    }
                    else if(key == KEY_PAGEDOWN)
					{
                        printk("PGDOWN\n");
                    }
                    else if(key == KEY_UP)
					{
                        printk("UP\n");
                    }
                    else if(key == KEY_DOWN)
					{
                        printk("DOWN\n");
                    }
					else if(key == KEY_BACKSPACE)
					{
						if(buffer_loc > 0)
						{
							cmd_buffer[buffer_loc-1] = ' ';
							buffer_loc--;

							vga_position_t pos = vga_get_position();
							pos.x--;
							vga_set_position(pos.x, pos.y);
							vga_putch(' ');
							vga_set_position(pos.x, pos.y);
						}
					}
					else
					{
						if(buffer_loc < 100)
						{
							cmd_buffer[buffer_loc++] = key;
							vga_putch((char)key);
						}
					}
				}
			}
		}
	}
	kbd->close();
}
