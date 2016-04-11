// syscall.h
// Userland syscall implementation

#ifndef INCLUDE_SYS_SYSCALL_H
#define INCLUDE_SYS_SYSCALL_H

#define DECL_SYSCALL0(fn) int syscall_##fn();
#define DECL_SYSCALL1(fn,p1) int syscall_##fn(p1);
#define DECL_SYSCALL2(fn,p1,p2) int syscall_##fn(p1,p2);
#define DECL_SYSCALL3(fn,p1,p2,p3) int syscall_##fn(p1,p2,p3);
#define DECL_SYSCALL4(fn,p1,p2,p3,p4) int syscall_##fn(p1,p2,p3,p4);
#define DECL_SYSCALL5(fn,p1,p2,p3,p4,p5) int syscall_##fn(p1,p2,p3,p4,p5);

//Processes
DECL_SYSCALL0(fork);
DECL_SYSCALL0(yield);
DECL_SYSCALL0(getpid);
DECL_SYSCALL1(exit, unsigned int);
DECL_SYSCALL1(sleep, unsigned int);

//Devices
DECL_SYSCALL1(open, char*);
DECL_SYSCALL1(close, unsigned int);
DECL_SYSCALL2(seek, unsigned int, unsigned int);
DECL_SYSCALL3(write, unsigned int, const void*, unsigned int);
DECL_SYSCALL3(read, unsigned int, void*, unsigned int);
DECL_SYSCALL3(ioctl, unsigned int, unsigned int, unsigned int);

//Memory (TEMP) - Needs sbrk and other so a userland malloc can be created
DECL_SYSCALL1(kmalloc, unsigned int);

DECL_SYSCALL1(debug, unsigned int);
DECL_SYSCALL3(read_hid, unsigned int, void*, unsigned int);

#endif
