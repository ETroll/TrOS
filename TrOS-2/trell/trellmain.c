#include <stdio.h>
#include <syscall.h>
#include <tros/tros.h>
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
    device_t kbd = syscall_open("kbd");

    // BOCHS_DEBUG;
    stdout = (file_t*)&vga;
    stdin = (file_t*)&kbd;

    printf("Test\n");
    while(TRUE)
    {
        printf("Test\n");
        BOCHS_DEBUG;
    }

    return 0;
}
