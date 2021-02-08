#pragma once

#include <abi/Syscalls.h>

#include <assert.h>

class Lock
{
private:
    static constexpr auto NO_HOLDER = 0xDEADDEAD;

    bool _locked = false;
    int _holder = NO_HOLDER;
    const char *_name = "lock-not-initialized";

    __SOURCE_LOCATION__ _last_acquire_location = INVALID_SOURCE_LOCATION;
    __SOURCE_LOCATION__ _last_release_location = INVALID_SOURCE_LOCATION;

    void ensure_failed(const char *raison, __SOURCE_LOCATION__ location)
    {
        __unused(raison);
        __unused(location);
        ASSERT_NOT_REACHED();
    }

public:
    bool locked() const
    {
        bool result;
        __atomic_load(&_locked, &result, __ATOMIC_SEQ_CST);
        return result;
    }

    int holder() const
    {
        int result;
        __atomic_load(&_holder, &result, __ATOMIC_SEQ_CST);
        return result;
    }

    const char *name() const
    {
        return _name;
    }

    __SOURCE_LOCATION__ acquire_location() { return _last_acquire_location; }

    __SOURCE_LOCATION__ release_location() { return _last_release_location; }

    constexpr Lock(const char *name) : _name{name}
    {
    }

    int process_this()
    {
        int pid = 0;
        hj_process_this(&pid);
        return pid;
    }

    void acquire(__SOURCE_LOCATION__ location)
    {
        acquire_for(process_this(), location);
    }

    void acquire_for(int holder, __SOURCE_LOCATION__ location)
    {
        while (!__sync_bool_compare_and_swap(&_locked, 0, 1))
        {
            asm("pause");
        }

        __sync_synchronize();
        _last_acquire_location = location;
        _holder = holder;
    }

    bool try_acquire(__SOURCE_LOCATION__ location)
    {
        return try_acquire_for(process_this(), location);
    }

    bool try_acquire_for(int holder, __SOURCE_LOCATION__ location)
    {
        if (__sync_bool_compare_and_swap(&_locked, 0, 1))
        {
            __sync_synchronize();

            _last_acquire_location = location;
            _holder = holder;

            return true;
        }
        else
        {
            __sync_synchronize();

            return false;
        }
    }

    void release(__SOURCE_LOCATION__ location)
    {
        release_for(process_this(), location);
    }

    void release_for(int holder, __SOURCE_LOCATION__ location)
    {
        ensure_acquired_for(holder, location);

        __sync_synchronize();

        __atomic_store_n(&_locked, 0, __ATOMIC_SEQ_CST);

        _last_release_location = location;
        _holder = NO_HOLDER;
    }

    void ensure_acquired(__SOURCE_LOCATION__ location)
    {
        ensure_acquired_for(process_this(), location);
    }

    void ensure_acquired_for(int holder, __SOURCE_LOCATION__ location)
    {
        if (!locked())
        {
            ensure_failed("lock-not-held", location);
            ASSERT_NOT_REACHED();
        }

        if (holder != this->holder())
        {
            ensure_failed("lock-held-by-someone-else", location);
        }
    }
};

class LockHolder
{
private:
    Lock &_lock;

public:
    LockHolder(Lock &lock) : _lock(lock)
    {
        _lock.acquire(SOURCE_LOCATION);
    }

    ~LockHolder()
    {
        _lock.release(SOURCE_LOCATION);
    }
};
