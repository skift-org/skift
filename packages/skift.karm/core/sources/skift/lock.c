/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/lock.h>

void __sk_lock_init(lock_t *lock)
{
    lock->locked = 0;
}

void __sk_lock_acquire(lock_t *lock)
{
    while (!__sync_bool_compare_and_swap(&lock->locked, 0, 1))
        asm("hlt"); // Don't burn the CPU ;)
    __sync_synchronize();
}

bool __sk_lock_try_acquire(lock_t *lock)
{
    while (!__sync_bool_compare_and_swap(&lock->locked, 0, 1))
        return false;
        
    __sync_synchronize();

    return true;
}

void __sk_lock_release(lock_t *lock)
{
    __sync_synchronize();
    lock->locked = 0;
}