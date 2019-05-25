/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/__plugs__.h>
#include <skift/lock.h>
#include <skift/assert.h>
#include <skift/process.h>

void __lock_init(lock_t *lock, const char* name)
{
    lock->locked = 0;
    lock->name = name;
}

void __lock_acquire(lock_t *lock)
{
    while (!__sync_bool_compare_and_swap(&lock->locked, 0, 1))
        asm("hlt"); // Don't burn the CPU ;)

    __sync_synchronize();
    
    lock->holder = process_this();
}

bool __lock_try_acquire(lock_t *lock)
{
    while (!__sync_bool_compare_and_swap(&lock->locked, 0, 1))
        return false;
        
    __sync_synchronize();

    lock->holder = process_this();

    return true;
}

void __lock_release(lock_t *lock)
{
    assert(lock->holder == process_this());

    __sync_synchronize();
    lock->holder = 0;
    lock->locked = 0;
}

void __lock_assert(lock_t *lock, const char* file, const char* function, int line)
{
    if (lock->holder != process_this() && !lock->locked)
    {
        __plug_lock_assert_failed(lock, file, function, line);
    }
}