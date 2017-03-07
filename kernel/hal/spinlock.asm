; spinlock.asm
; Platform dependent helper functions for spinlocks
; Copied from http://forum.osdev.org/viewtopic.php?f=1&t=28481&start=15

;                               +8
; void spinlock_lock_arch(spinlock_t lock);
global spinlock_lock_arch
spinlock_lock_arch:
    push ebp
    mov ebp,esp

    pushfd                      ;Put previous EFLAGS onto the stack
    cli                         ;Disable IRQs (in the hope that we do acquire the lock)
    lock bts dword [ebp+8], 0   ;Optimistically set bit 0 of "lock", and return the previous value in the carry flag
    jnc .acquired               ;Lock was acquired is bit 0 was previously clear

.retry:
    popfd                       ;Restore previous EFLAGS (enable IRQs if they were enabled before)
    pushfd                      ;Put previous EFLAGS onto the stack again

    pause                       ;Don't waste CPU resources
    bt dword [ebp+8], 0         ;Test if bit 0 of "lock" is already set
    jc .retry                   ;Retry (without doing the more expensive "lock bts [myLock],0") if bit 0 was set

    cli                         ;Disable IRQs (in the hope that we do acquire the lock)
    lock bts dword [ebp+8], 0   ;Set bit 0 of "lock", and return the previous value in the carry flag
    jc .retry                   ;Retry if bit 0 was already set

.acquired:
    mov eax, [esp]              ;lock[1] = previous EFLAGS
    mov dword [ebp+12], eax

    mov esp, ebp
    pop ebp
    ret

;                               +8
; void spinlock_unlock_arch(spinlock_t lock);
global spinlock_unlock_arch
spinlock_unlock_arch:
    push ebp
    mov ebp,esp

    mov dword [ebp+8], 0    ;Clear the lock
    push dword [ebp+12]     ;Put previous EFLAGS (still in lock[1] from acquiring the lock) onto stack
    popfd                   ;Restore previous EFLAGS (enable IRQs if they were enabled before)

    mov esp, ebp
    pop ebp
    ret
