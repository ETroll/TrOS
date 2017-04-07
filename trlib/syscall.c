#include <trlib/syscall.h>

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
DEFN_SYSCALL0(getpid, 0);
DEFN_SYSCALL0(getparentpid, 8);
DEFN_SYSCALL1(execute, 12, char**);
DEFN_SYSCALL1(exit, 13, unsigned int);

//Devices
DEFN_SYSCALL1(opendevice, 1, char*);
DEFN_SYSCALL1(closedevice, 2, unsigned int);
DEFN_SYSCALL3(writedevice, 3, unsigned int, const void*, unsigned int);
DEFN_SYSCALL3(readdevice, 4, unsigned int, void*, unsigned int);
DEFN_SYSCALL3(ioctl, 5, unsigned int, unsigned int, unsigned int);

//Memory
DEFN_SYSCALL1(increasemem, 9, unsigned int);
DEFN_SYSCALL1(decreasemem, 10, unsigned int);

//Messagequeues
DEFN_SYSCALL4(sendmessage, 6, unsigned int, const void*, unsigned int, unsigned int);
DEFN_SYSCALL3(readmessage, 7, const void*, unsigned int, unsigned int);


//Other temoporary or debug calls:
DEFN_SYSCALL1(debug, 11, unsigned int);
