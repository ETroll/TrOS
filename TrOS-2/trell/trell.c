#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>

#define BOCHS_DEBUG __asm__("xchgw %bx, %bx");

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
// }; http://forum.osdev.org/viewtopic.php?f=1&t=18333

file_t* stdout = NULL;
file_t* stdin = NULL;

int main()
{
    device_t vga = syscall_open("vga");
    //device_t kbd = syscall_open("kbd");

    // BOCHS_DEBUG;
    stdout = (file_t*)&vga;
    //stdin = (file_t*)&kbd;
    syscall_ioctl(vga, IOCTL_VGA_COLOR, 0xCF);
    syscall_ioctl(vga, IOCTL_VGA_CLEAR_MEM, 0);

    printf("Hello!\n");
    printf("Welcome to Trell - Your friendly servant!\n");

    char* buffer = (char*)malloc(5000*sizeof(char));
    printf("Allocated 5000bytes at %x\n", buffer);
    memset(buffer, '\0', 5000);
    printf("Cleared buffer\n");

    void* buffer2 = malloc(101);
    void* buffer3 = malloc(3000);
    void* buffer4 = malloc(1235);
    void* buffer5 = malloc(100000);

    free(buffer3);
    free(buffer5);
    free(buffer);
    free(buffer4);
    free(buffer2);

    printf("Freed all memory used\n");

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
