#pragma once

#include <embed/base.h>
#include <karm-meta/utils.h>

#include "atomic.h"
#include "string.h"

namespace Karm {

struct CriticalScope : Meta::NoCopy, Meta::NoMove {
    CriticalScope() {
        Embed::criticalEnter();
    }

    ~CriticalScope() {
        Embed::criticalLeave();
    }
};

struct Lock {
    Atomic<bool> _lock{};

    bool try_acquire() {
        Embed::criticalEnter();
        bool result = _lock.cmpxchg(false, true);
        memory_barier();

        if (!result) {
            Embed::criticalLeave();
        }

        return result;
    }

    void acquire() {
        Embed::criticalEnter();

        while (!try_acquire()) {
            Embed::relaxe();
        }
    }

    void release() {
        memory_barier();
        _lock.store(false);

        Embed::criticalLeave();
    }
};

struct LockScope : Meta::NoCopy, Meta::NoMove {
    Lock &_lock;

    LockScope(Lock &lock) : _lock(lock) {
        _lock.acquire();
    }

    ~LockScope() {
        _lock.release();
    }
};

} // namespace Karm
