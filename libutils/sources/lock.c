#include "sync/lock.h"

void lock_init(lock_t* lock, string name)
{
    lock->locked = false;
    lock->name = name;
}

void lock_acquire(lock_t* lock)
{
    while (!__sync_bool_compare_and_swap(&lock->locked, false, true));
    __sync_synchronize();
}

void lock_release(lock_t* lock)
{
    __sync_synchronize();
    lock->locked = false;
}