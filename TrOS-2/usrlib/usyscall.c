#include <syscall.h>

#define DEFN_SYSCALL0(fn, num) \
int syscall_##fn() \
{ \
    int a; \
    __asm("int $0x80" : "=a" (a) : "0" (num)); \
    return a; \
}

#define DEFN_SYSCALL1(fn, num, P1) \
int syscall_##fn(P1 p1) \
{ \
    int a; \
    __asm("int $0x80" : "=a" (a) : "0" (num), "b" ((int)p1)); \
    return a; \
}

#define DEFN_SYSCALL2(fn, num, P1, P2) \
int syscall_##fn(P1 p1, P2 p2) \
{ \
    int a; \
    __asm("int $0x80" : "=a" (a) : "0" (num), "b" ((int)p1), "c" ((int)p2)); \
    return a; \
}

#define DEFN_SYSCALL3(fn, num, P1, P2, P3) \
int syscall_##fn(P1 p1, P2 p2, P3 p3) \
{ \
    int a; \
    __asm("int $0x80" : "=a" (a) : "0" (num), "b" ((int)p1), "c" ((int)p2), "d"((int)p3)); \
    return a; \
}

#define DEFN_SYSCALL4(fn, num, P1, P2, P3, P4) \
int syscall_##fn(P1 p1, P2 p2, P3 p3, P4 p4) \
{ \
    int a; \
    __asm("int $0x80" : "=a" (a) : "0" (num), "b" ((int)p1), "c" ((int)p2), "d" ((int)p3), "S" ((int)p4)); \
    return a; \
}

#define DEFN_SYSCALL5(fn, num) \
int syscall_##fn(P1 p1, P2 p2, P3 p3, P4 p4, P5 p5) \
{ \
    int a; \
    __asm("int $0x80" : "=a" (a) : "0" (num), "b" ((int)p1), "c" ((int)p2), "d" ((int)p3), "S" ((int)p4), "D" ((int)p5)); \
    return a; \
}

//Processes
DEFN_SYSCALL0(fork, 0);
DEFN_SYSCALL0(yield, 1);
DEFN_SYSCALL0(getpid, 2);
DEFN_SYSCALL1(exit, 3, unsigned int);
DEFN_SYSCALL1(sleep, 4, unsigned int);

//Devices
DEFN_SYSCALL1(open, 5, char*);
DEFN_SYSCALL1(close, 6, unsigned int);
DEFN_SYSCALL1(peek, 7, unsigned int);
DEFN_SYSCALL3(write, 8, unsigned int, const void*, unsigned int);
DEFN_SYSCALL3(read, 9, unsigned int, void*, unsigned int);

//Memory (TEMP) - Needs sbrk and other so a userland malloc can be created
DEFN_SYSCALL1(kmalloc, 10, unsigned int);

//Other temoporary or debug calls:
DEFN_SYSCALL0(debug, 11);
DEFN_SYSCALL3(read_hid, 12, unsigned int, void*, unsigned int);
