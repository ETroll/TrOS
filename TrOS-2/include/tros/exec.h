
#ifndef INCLUDE_TROS_EXEC_H
#define INCLUDE_TROS_EXEC_H

/**
    Executes a ELF32 file. (Creates a new process and its primary thread)
*/
int exec_elf32(char* path, int argc, char** argv);

#endif
