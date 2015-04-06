#ifndef _SYSCALLS_H
#define _SYSCALLS_H

extern void yield(void);
extern int fork(void);
extern void exit(unsigned int);
extern void sleep(unsigned int);
extern unsigned int getpid(void);


extern int open(char* path);
extern int close(int fd);
extern int peek(int fd);
extern int write(int fd, const void *buffer, unsigned int count);
extern int read(int fd, void *buffer, unsigned int count);



#endif
