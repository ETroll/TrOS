
#include <trlib/device.h>
#include <trlib/system.h>
#include <trlib/mq.h>
#include <stdio.h>
#include <string.h>
#include <keycodes.h>
#include "ui/ui.h"
#include "ui/list.h"
#include "windows/syslog.h"
#include "windows/showcase.h"

#define BOCHS_DEBUG __asm__("xchgw %bx, %bx");

// char *builtin_str[] = {
//     "cd",
//     "help"
// };
//
// int (*builtin_func[]) (char **) = {
//     &lsh_cd,
//     &lsh_help
// };

file_t* stdout = NULL;
file_t* stdin = NULL;

int main()
{
    ui_context_t* context = ui_context_create("vga");
    if(context)
    {
        ui_window_t* showcase = showcase_create();
        ui_window_t* syslog = syslog_create();
        ui_desktop_t* desktop = ui_desktop_create(context);

        list_add(desktop->windows, showcase);
        list_add(desktop->windows, syslog);

        ui_desktop_set_activewindow(desktop, showcase);
        ui_redraw(desktop);

        device_t kbd = device_open("kbd");

        int32_t cr3 = system_debug(DEBUG_CR3);
        syslog_log(1, SYSLOG_INFO, "CR3 %x", cr3);

        while(1)
        {
            int key = 0;
            char buffer[20];

            if(mq_recv(buffer, 20, MQ_NOFLAGS) > 0)
            {
                syslog_log(1, SYSLOG_INFO, "Got message: %s", buffer);
            }

            device_readdata(kbd, &key, 1);

            if(key >= KEY_F1 && key < KEY_F9)
            {
                uint32_t index = key - KEY_F1;
                ui_window_t* win = (ui_window_t*)list_get_at(desktop->windows, index);
                if(win != NULL)
                {
                    ui_desktop_set_activewindow(desktop, win);
                }
                else
                {
                    syslog_log(1, SYSLOG_INFO, "No window found at index %d", index);
                }
            }
            else
            {
                if(desktop->activeWindow->handlemessage != NULL)
                {
                    desktop->activeWindow->handlemessage(UI_KEYSTROKE, key, desktop->activeWindow);
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
