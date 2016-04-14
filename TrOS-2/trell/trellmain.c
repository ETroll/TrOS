#include <stdio.h>
#include <syscall.h>
#include <tros/tros.h>

#define IOCTL_VGA_COLOR         1
#define IOCTL_VGA_SCROLL_UP     2
#define IOCTL_VGA_SCROLL_DOWN   3
#define IOCTL_VGA_TOGGLE_CURSOR 4
#define IOCTL_VGA_CLEAR_MEM     5
#define IOCTL_VGA_SHOULD_SCROLL 6

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

int trell_main()
{
    device_t vga = syscall_open("vga");
    //device_t kbd = syscall_open("kbd");

    // BOCHS_DEBUG;
    stdout = (file_t*)&vga;
    //stdin = (file_t*)&kbd;

    syscall_ioctl(vga, IOCTL_VGA_CLEAR_MEM, 0);

    printf("Hello!\n");
    printf("Welcome to Trell - Your servant for the evening!\n");
    while(TRUE)
    {
        BOCHS_DEBUG;
    }

    return 0;
}
