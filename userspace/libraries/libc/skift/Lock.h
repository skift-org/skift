#pragma once

#include <abi/Syscalls.h>

#include <assert.h>

#include <libutils/SourceLocation.h>

#ifdef __KERNEL__
#    include "kernel/interrupts/Interupts.h"
#endif

class Lock
{
private:
    static constexpr auto NO_HOLDER = 0xDEADDEAD;

    bool _locked = false;
    int _holder = NO_HOLDER;
    const char *_name = "lock-not-initialized";

    Utils::SourceLocation _last_acquire_location{};
    Utils::SourceLocation _last_release_location{};

    NONMOVABLE(Lock);
    NONCOPYABLE(Lock);

    void ensure_failed(const char *raison, Utils::SourceLocation location)
    {
        UNUSED(raison);
        UNUSED(location);
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

    Utils::SourceLocation acquire_location() { return _last_acquire_location; }

    Utils::SourceLocation release_location() { return _last_release_location; }

    constexpr Lock(const char *name) : _name{name}
    {
    }

    int process_this()
    {
        int pid = 0;
        hj_process_this(&pid);
        return pid;
    }

    void acquire(Utils::SourceLocation location = Utils::SourceLocation::current())
    {
        acquire_for(process_this(), location);
    }

    void acquire_for(int holder, Utils::SourceLocation location = Utils::SourceLocation::current())
    {
        while (!__sync_bool_compare_and_swap(&_locked, 0, 1))
        {

#ifdef __KERNEL__
            ASSERT_INTERRUPTS_NOT_RETAINED();
#endif

            asm("pause");
        }

        __sync_synchronize();
        _last_acquire_location = location;
        _holder = holder;
    }

    bool try_acquire(Utils::SourceLocation location = Utils::SourceLocation::current())
    {
        return try_acquire_for(process_this(), location);
    }

    bool try_acquire_for(int holder, Utils::SourceLocation location = Utils::SourceLocation::current())
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

    void release(Utils::SourceLocation location = Utils::SourceLocation::current())
    {
        release_for(process_this(), location);
    }

    void release_for(int holder, Utils::SourceLocation location = Utils::SourceLocation::current())
    {
        ensure_acquired_for(holder, location);

        __sync_synchronize();

        __atomic_store_n(&_locked, 0, __ATOMIC_SEQ_CST);

        _last_release_location = location;
        _holder = NO_HOLDER;
    }

    void ensure_acquired(Utils::SourceLocation location = Utils::SourceLocation::current())
    {
        ensure_acquired_for(process_this(), location);
    }

    void ensure_acquired_for(int holder, Utils::SourceLocation location = Utils::SourceLocation::current())
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
    LockHolder(Lock &lock, Utils::SourceLocation location = Utils::SourceLocation::current()) : _lock(lock)
    {
        _lock.acquire(location);
    }

    ~LockHolder()
    {
        _lock.release();
    }
};
