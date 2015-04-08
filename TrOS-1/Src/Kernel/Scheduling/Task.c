#include "Task.h"
#include <Tros/Utils.h>

// Initializes and sets up a new stack
unsigned int* task_stack_init(unsigned int *stack, void (*start)(void))
{
    _k_memset(stack, 0x00, STACK_SIZE);
    stack += STACK_SIZE - 16;
    stack[0] = 0x10; //User mode, interrupts on
    stack[1] = (unsigned int)start;
    return stack;
}

// Copies the stack and returns a pointer to the SP address for
// the stack. Used in forking.
unsigned int* task_stack_copy(unsigned int* old, unsigned int* new, unsigned int n)
{
    //Create a pointer to the where the old SP would be
    unsigned int* new_sp = new + STACK_SIZE - n;

    //Copy the used part of the old stack
    _k_memcpy(new_sp, old, n*sizeof(*old));

    return new_sp;
}

// Only needed as long as we dont have dynamic memory allocation
// since we are using pre-allocated stacks for the task allocated on the
// kernel stack.
int task_nextfree(struct task *status, int limit)
{
    int nextfree = -1;
    for (int i = 0; i < limit; i++)
    {
        if (status[i].status == TASK_DEAD)
        {
            nextfree = i;
        }
    }
    return nextfree;
}
