/* Copyright © 2018-2019 N. Van Bossuyt.                                               */
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
        ;
    __sync_synchronize();
}

void __sk_lock_release(lock_t *lock)
{
    __sync_synchronize();
    lock->locked = 0;
}