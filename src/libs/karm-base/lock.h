#pragma once

#include <embed/base.h>
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

struct Lock {
    Atomic<bool> _lock{};

    bool _tryAcquire() {
        bool result = _lock.cmpxchg(false, true);
        memory_barier();

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
        memory_barier();
        _lock.store(false);

        Embed::leaveCritical();
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
