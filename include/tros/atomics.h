
#ifndef INCLUDE_TROS_ATOMICS_H
#define INCLUDE_TROS_ATOMICS_H

typedef volatile int spinlock_t[2];

void spinlock_inistialize(spinlock_t lock);

void spinlock_lock(spinlock_t lock);
void spinlock_unlock(spinlock_t lock);

#endif
