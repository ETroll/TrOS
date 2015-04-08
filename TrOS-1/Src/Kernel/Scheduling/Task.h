#ifndef _TASK_H
#define _TASK_H

#define STACK_SIZE 1024
#define TASK_LIMIT 50

#define TASK_RUNNING 0x0
#define TASK_SLEEPING 0x1
#define TASK_DEAD 0x2
#define TASK_WAITING_READ 0x3
#define TASK_WAITING_WRITE 0x4

//Not really task related, but kept here for now
#define PIPE_LIMIT (TASK_LIMIT*5)
#define PATH_MAX 255
#define PATHSERVER_FD (TASK_LIMIT+3)

struct task
{
    int PID;
    unsigned int* stack; //current HEAD, or SP
    unsigned int task_id; //ID in the task array
    unsigned int status;
    struct task* next;
    struct task* prev;
    struct task* parent;
};


unsigned int* task_stack_init(unsigned int *stack, void (*start)(void));
unsigned int* task_stack_copy(unsigned int* old, unsigned int* new, unsigned int n);
int task_nextfree(struct task *status, int limit);

extern unsigned int* activate(unsigned int*);

#endif
