/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>
#include <libsystem/lock.h>
#include <libsystem/assert.h>
#include <libsystem/process.h>
#include <libsystem/logger.h>

void __lock_init(lock_t *lock, const char *name)
{
    lock->locked = 0;
    lock->name = name;
    lock->holder = 939393;
}

void __lock_acquire(lock_t *lock)
{
    while (!__sync_bool_compare_and_swap(&lock->locked, 0, 1))
        asm("hlt"); // Don't burn the CPU ;)

    __sync_synchronize();

    lock->holder = process_this();
}

void __lock_acquire_by(lock_t *lock, int holder)
{
    while (!__sync_bool_compare_and_swap(&lock->locked, 0, 1))
        asm("hlt"); // Don't burn the CPU ;)

    __sync_synchronize();

    lock->holder = holder;
}

bool __lock_try_acquire(lock_t *lock)
{
    while (!__sync_bool_compare_and_swap(&lock->locked, 0, 1))
        return false;

    __sync_synchronize();

    lock->holder = process_this();

    return true;
}

void __lock_release(lock_t *lock, const char *file, const char *function, int line)
{
    __lock_assert(lock, file, function, line);

    __sync_synchronize();

    lock->holder = 0;
    lock->locked = 0;
}

void __lock_assert(lock_t *lock, const char *file, const char *function, int line)
{
    if (lock->holder != process_this() && !lock->locked)
    {
        logger_error("The thread holding(%d) the lock %s isn't the same has the one releasing(%d) it!", lock->holder, lock->name, process_this());
        __plug_lock_assert_failed(lock, file, function, line);
    }
}