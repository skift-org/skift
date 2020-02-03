/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/__plugs__.h>
#include <libsystem/assert.h>
#include <libsystem/lock.h>
#include <libsystem/logger.h>
#include <libsystem/process/Process.h>

void __lock_init(Lock *lock, const char *name)
{
    lock->locked = 0;
    lock->name = name;
    lock->holder = 0x939393;
}

void __lock_acquire(Lock *lock)
{
    while (!__sync_bool_compare_and_swap(&lock->locked, 0, 1))
        asm("hlt"); // Don't burn the CPU ;)

    __sync_synchronize();

    lock->holder = process_this();
}

void __lock_acquire_by(Lock *lock, int holder)
{
    while (!__sync_bool_compare_and_swap(&lock->locked, 0, 1))
        asm("hlt"); // Don't burn the CPU ;)

    __sync_synchronize();

    lock->holder = holder;
}

bool __lock_try_acquire(Lock *lock)
{
    while (!__sync_bool_compare_and_swap(&lock->locked, 0, 1))
        return false;

    __sync_synchronize();

    lock->holder = process_this();

    return true;
}

void __lock_release(Lock *lock, const char *file, const char *function, int line)
{
    __lock_assert(lock, file, function, line);

    __sync_synchronize();

    lock->holder = 0;
    lock->locked = 0;
}

void __lock_release_by(Lock *lock, int holder, const char *file, const char *function, int line)
{
    if (!lock->locked)
    {
        logger_error("Thread(%d) try to release the lock %s bus wasn't lock in the first place!", holder, lock->name);
        __plug_lock_assert_failed(lock, file, function, line);
    }

    if (lock->holder != holder)
    {
        logger_error("The thread(%d) holding the lock %s isn't the same has the one releasing(%d) it!", lock->holder, lock->name, holder);
        __plug_lock_assert_failed(lock, file, function, line);
    }

    __sync_synchronize();

    lock->holder = 0;
    lock->locked = 0;
}

void __lock_assert(Lock *lock, const char *file, const char *function, int line)
{
    if (lock->holder != process_this() && !lock->locked)
    {
        logger_error("The thread(%d) holding the lock %s isn't the same has the one releasing(%d) it!", lock->holder, lock->name, process_this());
        __plug_lock_assert_failed(lock, file, function, line);
    }
}