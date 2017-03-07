// Spinlocks
//
// - http://forum.osdev.org/viewtopic.php?t=28481
// - http://forum.osdev.org/viewtopic.php?f=1&t=28481&start=15
// - http://wiki.osdev.org/Spinlock

#include <tros/atomics.h>

extern void spinlock_lock_arch(spinlock_t lock);
extern void spinlock_unlock_arch(spinlock_t lock);

void spinlock_inistialize(spinlock_t lock)
{
    lock[0] = 0;
    lock[1] = 0;
}

void spinlock_lock(spinlock_t lock)
{
    spinlock_lock_arch(lock);
}

void spinlock_unlock(spinlock_t lock)
{
    spinlock_unlock_arch(lock);
}
