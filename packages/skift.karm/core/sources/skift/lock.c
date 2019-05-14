/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/__plugs__.h>
#include <skift/lock.h>
#include <skift/assert.h>

void __lock_init(lock_t *lock)
{
    lock->locked = 0;
}

void __lock_acquire(lock_t *lock)
{
    while (!__sync_bool_compare_and_swap(&lock->locked, 0, 1))
        asm("hlt"); // Don't burn the CPU ;)

    __sync_synchronize();
    
    lock->holder = __plug_thread_this();
}

bool __lock_try_acquire(lock_t *lock)
{
    while (!__sync_bool_compare_and_swap(&lock->locked, 0, 1))
        return false;
        
    __sync_synchronize();

    return true;
}

void __lock_release(lock_t *lock)
{
    assert(lock->holder == __plug_thread_this());

    __sync_synchronize();
    lock->holder = 0;
    lock->locked = 0;
}