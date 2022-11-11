#pragma once

#include <embed/base.h>
#include <karm-meta/utils.h>

#include "atomic.h"
#include "string.h"

namespace Karm {

struct CriticalScope : Meta::Static {
    CriticalScope() {
        Embed::criticalEnter();
    }

    ~CriticalScope() {
        Embed::criticalLeave();
    }
};

struct Lock {
    Atomic<bool> _lock{};

    bool _tryAcquire() {
        bool result = _lock.cmpxchg(false, true);
        memory_barier();

        if (!result) {
            Embed::criticalLeave();
        }

        return result;
    }

    bool tryAcquire() {
        Embed::criticalEnter();
        return _tryAcquire();
    }

    void acquire() {
        Embed::criticalEnter();

        while (!_tryAcquire()) {
            Embed::relaxe();
        }
    }

    void release() {
        memory_barier();
        _lock.store(false);

        Embed::criticalLeave();
    }
};

struct LockScope :
    Meta::Static {
    Lock &_lock;

    LockScope(Lock &lock) : _lock(lock) {
        _lock.acquire();
    }

    ~LockScope() {
        _lock.release();
    }
};

} // namespace Karm
