#include "Terminal.h"

/*
 * A "dummy" terminal to act as a temporary mean of communicating
 * with the OS until a real terminal have been implemented.
 */


#define VT_BACKSPACE 0x08
#define VT_DELTETE 0x7F
#define VT_ESCAPECHAR 0x1b

extern void lcd_main(void);
extern void memory_main();

void terminal_handle_command(char* cmd);

void terminal_main()
{
	fprintf(stdout,  "-------------------------\n");
	fprintf(stdout, "TrOS v0.0.1:\n");
	fprintf(stdout, "> ");


	char buf[254];
	int counter = 0;

	while(1)
	{
		int len = peek(*stdin);
		if(len > 0)
		{
			read(*stdin, &buf[counter], 1);
			//fprintf(stdout, "key: %x  \n", buf[counter]);
			switch(buf[counter])
			{
				case '\n':
				{
					buf[counter] = '\0';
					terminal_handle_command(buf);
					counter = 0;
					fprintf(stdout, "\n> ");
				}break;
				case 0x7F:
				{
					if(counter > 0)
					{
						buf[counter--] = '\0';
						fputc(0x08, stdout);
						fputc(0x7f, stdout);
					}
				}break;
				default:
					fputc(buf[counter], stdout);
					counter++;
					break;
			}
		}
	}
}

void terminal_handle_command(char* cmd)
{
	if(strcmp(cmd, "lcdtest\0") == 0)
	{
		if(!fork()) lcd_main();
	}
	else if(strcmp(cmd, "memtest") == 0)
	{
		memory_main();
	}
	else if(strcmp(cmd, "getpid") == 0)
	{
		fprintf(stdout, "Current PID: %d\n", getpid());
	}
	else if(strcmp(cmd, "tasks") == 0)
	{
		fprintf(stdout, "Running processes.. TODO!\n");
	}
	else
	{
		fprintf(stdout, "\nUnknown command\n");
	}
}
