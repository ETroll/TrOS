#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include "ui/ui.h"
#include "builtins/syslog.h"

#define BOCHS_DEBUG __asm__("xchgw %bx, %bx");

// char *builtin_str[] = {
//     "cd",
//     "help"
// };
//
// int (*builtin_func[]) (char **) = {
//     &lsh_cd,
//     &lsh_help
// }; http://forum.osdev.org/viewtopic.php?f=1&t=18333

file_t* stdout = 0;
file_t* stdin = 0;

int main()
{
    ui_context_t* context = ui_context_create("vga");
    if(context)
    {
        ui_window_t* window = ui_window_create("Window 1");
        ui_window_t* syslog = syslog_create();
        ui_desktop_t* desktop = ui_desktop_create(context);

        list_add(desktop->windows, window);
        list_add(desktop->windows, syslog);
        desktop->activeWindow = window;

        ui_redraw(desktop);

        int32_t kbd = syscall_opendevice("kbd");
        while(1)
        {
            int key = 0;
            syscall_readdevice(kbd, &key, 1);

            if(key > 0x1200 && key < 0x1209)
            {
                if(key == 0x1203 && desktop->activeWindow != syslog)
                {
                    desktop->activeWindow = syslog;
                }
                else
                {
                    desktop->activeWindow = window;
                }
            }
            else
            {
                if(desktop->activeWindow->inputhandler != NULL)
                {
                    desktop->activeWindow->inputhandler(key);
                }
            }
            ui_redraw(desktop);
        }
    }


    /*
    #include <stdio.h>
    #include <dirent.h>

    int main()
    {
        DIR *dir;
        struct dirent *dp;
        char * file_name;
        dir = opendir(".");
        while ((dp=readdir(dir)) != NULL) {
            printf("debug: %s\n", dp->d_name);
            if ( !strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..") )
            {
                // do nothing (straight logic)
            } else {
                file_name = dp->d_name; // use it
                printf("file_name: \"%s\"\n",file_name);
            }
        }
        closedir(dir);
        return 0;
    }
    */

    while(TRUE)
    {
        BOCHS_DEBUG;
    }

    return 0;
}
