#include <stdio.h>
#include <string.h>
#include <trlib/system.h>
#include <trlib/mq.h>
#include <trlib/trui.h>

file_t* stdout = NULL;
file_t* stdin = NULL;

int main(int argc, char** argv)
{
    int parent = system_parentpid();




    system_exit(1);
    return 0;
}
