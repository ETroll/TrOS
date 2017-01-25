// stdio.h
// C stdio implementation

#ifndef INCLUDE_STDIO_H
#define INCLUDE_STDIO_H

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
typedef int device_t;

extern file_t* stdout;
extern file_t* stdin;

int atoi(const char* str);

void printf(char* str, ...);
int fputc (int character, file_t* file);
int fputs (const char* str, file_t* file );

void printf(char* str, ...);

// file_t *fopen(const char *filename, const char *mode);
// void fclose(file_t file);

#endif
