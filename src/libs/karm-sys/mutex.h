#pragma once

#include <karm-base/rc.h>
#include <karm-base/result.h>

namespace Karm::Sys {

struct Mutext {
    Result<Strong<Mutext>> create();

    virtual ~Mutext() = default;

    virtual void lock() = 0;

    virtual void tryLock() = 0;

    virtual void unlock() = 0;
};

struct Sema {
    Result<Strong<Sema>> create();

    virtual ~Sema() = default;

    virtual void wait() = 0;

    virtual void tryWait() = 0;

    virtual void signal(size_t n = 1) = 0;

    virtual void count() = 0;
};

struct CondVar {
    Result<Strong<CondVar>> create();

    virtual ~CondVar() = default;

    virtual void wait() = 0;

    virtual void signal() = 0;

    virtual void broadcast() = 0;
};

} // namespace Karm::Sys
