#pragma once

#include <karm-meta/nocopy.h>

#include "_embed.h"

#include "atomic.h"
#include "string.h"

namespace Karm {

struct [[nodiscard]] CriticalScope :
    Meta::Pinned {

    CriticalScope() {
        _Embed::enterCritical();
    }

    ~CriticalScope() {
        _Embed::leaveCritical();
    }
};

struct Lock :
    Meta::Pinned {

    Atomic<bool> _lock{};

    bool _tryAcquire() {
        bool result = _lock.cmpxchg(false, true);
        memoryBarier();

        if (not result)
            _Embed::leaveCritical();

        return result;
    }

    bool tryAcquire() {
        _Embed::enterCritical();
        return _tryAcquire();
    }

    void acquire() {
        _Embed::enterCritical();

        while (not _tryAcquire())
            _Embed::relaxe();
    }

    void release() {
        memoryBarier();
        _lock.store(false);
        _Embed::leaveCritical();
    }
};

struct NoLock : Meta::Pinned {
    bool tryAcquire() {
        return true;
    }

    void acquire() {}

    void release() {}
};

template <typename T>
concept Lockable =
    requires(T &lockable) {
        lockable.tryAcquire();
        lockable.acquire();
        lockable.release();
    };

template <Lockable L = Lock>
struct [[nodiscard]] LockScope :
    Meta::Pinned {
    L &_lock;

    LockScope(L &lock)
        : _lock(lock) {
        _lock.acquire();
    }

    ~LockScope() {
        _lock.release();
    }
};

template <Lockable L>
LockScope(L &) -> LockScope<L>;

template <typename T, Lockable L = Lock>
struct LockProtected {
    L _lock;
    T _value;

    LockProtected() = default;

    template <typename... Args>
    LockProtected(Args &&...args)
        : _value(std::forward<Args>(args)...) {}

    auto with(auto f) {
        LockScope scope(_lock);
        return f(_value);
    }
};

template <typename T, Lockable L = Lock>
LockProtected(T, L &) -> LockProtected<T, L>;

struct RwLock : Meta::Pinned {
    Lock _lock;
    Atomic<isize> _pendings{};
    isize _readers{};
    isize _writers{};

    void acquireRead() {
        _Embed::enterCritical();

        while (not tryAcquireRead()) {
            _Embed::relaxe();
            memoryBarier();
        }
    }

    bool tryAcquireRead() {
        LockScope scope(_lock);

        if (_pendings.load())
            return false;

        if (_writers)
            return false;

        ++_readers;

        _Embed::enterCritical();
        return true;
    }

    void releaseRead() {
        LockScope scope(_lock);
        --_readers;
        _Embed::leaveCritical();
    }

    void acquireWrite() {
        _Embed::enterCritical();

        _pendings.inc();

        while (not tryAcquireWrite()) {
            _Embed::relaxe();
            memoryBarier();
        }

        _pendings.dec();
    }

    bool tryAcquireWrite() {
        LockScope scope(_lock);

        if (_readers)
            return false;

        if (_writers)
            return false;

        ++_writers;
        _Embed::enterCritical();
        return true;
    }

    void releaseWrite() {
        LockScope scope(_lock);
        --_writers;
        _Embed::leaveCritical();
    }
};

struct [[nodiscard]] ReadLockScope : Meta::Pinned {
    RwLock &_lock;

    ReadLockScope(RwLock &lock)
        : _lock(lock) {
        _lock.acquireRead();
    }

    ~ReadLockScope() {
        _lock.releaseRead();
    }
};

struct [[nodiscard]] WriteLockScope : Meta::Pinned {
    RwLock &_lock;

    WriteLockScope(RwLock &lock)
        : _lock(lock) {
        _lock.acquireWrite();
    }

    ~WriteLockScope() {
        _lock.releaseWrite();
    }
};

} // namespace Karm
