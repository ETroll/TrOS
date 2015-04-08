#ifndef SRC_INCLUDE_STDIO_H_
#define SRC_INCLUDE_STDIO_H_

typedef int file_t;
typedef int device_t;

extern file_t* stdout;
extern file_t* stdin;

int fprintf(file_t* file, char* str, ...);
int fputc (int character, file_t* file);
int fputs (const char* str, file_t* file );

#define NULL	0
#define TRUE    1
#define FALSE   0

#endif /* SRC_INCLUDE_STDIO_H_ */
