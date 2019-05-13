#pragma once

/* Copyright © 2018-2019 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <skift/runtime.h>

typedef struct
{
    int locked;
    int holder;
} lock_t;

void __lock_init(lock_t *lock);
void __lock_acquire(lock_t *lock);
void __lock_release(lock_t *lock);
bool __lock_try_acquire(lock_t *lock);

#define lock_init(lock) __lock_init(&lock)
#define lock_acquire(lock) __lock_acquire(&lock)
#define lock_try_acquire(lock) __lock_try_acquire(&lock)
#define lock_release(lock) __lock_release(&lock)

#define LOCK(lock, code)       \
    do                         \
    {                          \
        __lock_acquire(&lock); \
        code;                  \
        __lock_release(&lock); \
    } while (0);
