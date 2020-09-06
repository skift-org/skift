#pragma once

#include <libsystem/Common.h>

struct Lock
{
    bool locked;
    int holder;
    const char *name;
};

void __lock_init(Lock *lock, const char *name);

void __lock_acquire(Lock *lock);

void __lock_acquire_by(Lock *lock, int holder);

void __lock_release(Lock *lock, const char *file, const char *function, int line);

void __lock_release_by(Lock *lock, int holder, const char *file, const char *function, int line);

bool __lock_try_acquire(Lock *lock);

void __lock_assert(Lock *lock, const char *file, const char *function, int line);

#define lock_init(lock) __lock_init(&lock, #lock)

#define lock_acquire(lock) __lock_acquire(&lock)

#define lock_acquire_by(lock, __holder) __lock_acquire_by(&lock, __holder)

#define lock_try_acquire(lock) __lock_try_acquire(&lock)

#define lock_release(lock) __lock_release(&lock, __FILE__, __FUNCTION__, __LINE__)

#define lock_release_by(lock, __holder) __lock_release_by(&lock, __holder, __FILE__, __FUNCTION__, __LINE__)

#define lock_assert(lock) __lock_assert(&lock, __FILE__, __FUNCTION__, __LINE__)

#define lock_is_acquire(__lock) ((&__lock)->locked)

class LockHolder
{
private:
    Lock &_lock;

public:
    LockHolder(Lock &lock) : _lock(lock)
    {
        lock_acquire(_lock);
    }

    ~LockHolder()
    {
        lock_release(_lock);
    }
};
