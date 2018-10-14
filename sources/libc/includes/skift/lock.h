#pragma once

typedef struct
{
    int locked;
} lock_t;

void __sk_lock_init(lock_t *lock);
void __sk_lock_acquire(lock_t *lock);
void __sk_lock_release(lock_t *lock);

#define sk_lock_init(lock) __sk_lock_init(&lock)
#define sk_lock_acquire(lock) __sk_lock_acquire(&lock)
#define sk_lock_release(lock) __sk_lock_release(&lock)

#define LOCK(lock, code)         \
    do                           \
    {                            \
        __sk_lock_acquire(&lock); \
        code;                    \
        __sk_lock_release(&lock); \
    } while (0);
