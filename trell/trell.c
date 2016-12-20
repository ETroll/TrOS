#include <stdio.h>
#include <string.h>
#include <syscall.h>
#include "ui.h"

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
        ui_window_t* window = ui_window_create("Test Window 1", 1, 1, 78, 22, context);
        ui_window_t* window2 = ui_window_create("Test Window 2", 10, 10, 30, 8, context);
        // ui_window_t* toolbar = ui_window_create(0, 24, 80, 1, context);
        //ui_window_t* window = ui_window_create(1, 6, 10, 1, context);


        // ui_window_paint(toolbar);


        int32_t kbd = syscall_opendevice("kbd");
        uint32_t count = 0;
        while(1)
        {
            int key = 0;
            syscall_readdevice(kbd, &key, 1);

            if(count == 0)
            {
                ui_window_paint(window);
                count++;
            }
            else
            {
                ui_window_paint(window2);
            }
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
