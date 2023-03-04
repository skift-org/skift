#pragma once

#include <embed-base/base.h>
#include <karm-meta/nocopy.h>

#include "atomic.h"
#include "string.h"

namespace Karm {

struct CriticalScope : Meta::Static {
    CriticalScope() {
        Embed::enterCritical();
    }

    ~CriticalScope() {
        Embed::leaveCritical();
    }
};

struct Lock : Meta::Static {
    Atomic<bool> _lock{};

    bool _tryAcquire() {
        bool result = _lock.cmpxchg(false, true);
        memoryBarier();

        if (not result) {
            Embed::leaveCritical();
        }

        return result;
    }

    bool tryAcquire() {
        Embed::enterCritical();
        return _tryAcquire();
    }

    void acquire() {
        Embed::enterCritical();

        while (not _tryAcquire()) {
            Embed::relaxe();
        }
    }

    void release() {
        memoryBarier();
        _lock.store(false);
        Embed::leaveCritical();
    }
};

struct NoLock : Meta::Static {
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

template <Lockable L>
struct LockScope :
    Meta::Static {
    L &_lock;

    LockScope(L &lock)
        : _lock(lock) {
        _lock.acquire();
    }

    ~LockScope() {
        _lock.release();
    }
};

} // namespace Karm
