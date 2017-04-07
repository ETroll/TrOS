// stdio.h
// C stdio implementation

#ifndef INCLUDE_STDIO_H
#define INCLUDE_STDIO_H

#include <stdarg.h>

#ifndef NULL
#define NULL	0
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

typedef int file_t;

extern file_t* stdout;
extern file_t* stdin;

int atoi(const char* str);

int fputc(char c, void* file);
int fputs(const char* str, void* file);

void printf(char* str, ...);
void vprintf(char* str, va_list va);

void sprintf(char* str, char* format, ...);
void vsprintf(char* str, char* format, va_list va);

// file_t *fopen(const char *filename, const char *mode);
// void fclose(file_t file);

#endif
