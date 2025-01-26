#pragma once

#include <karm-base/rc.h>
#include <karm-base/res.h>

namespace Karm::Sys {

struct Mutex {
    Res<Rc<Mutex>> create();

    virtual ~Mutex() = default;

    virtual void lock() = 0;

    virtual void tryLock() = 0;

    virtual void unlock() = 0;
};

struct Sema {
    Res<Rc<Sema>> create();

    virtual ~Sema() = default;

    virtual void wait() = 0;

    virtual void tryWait() = 0;

    virtual void signal(usize n = 1) = 0;

    virtual void count() = 0;
};

struct CondVar {
    Res<Rc<CondVar>> create();

    virtual ~CondVar() = default;

    virtual void wait() = 0;

    virtual void signal() = 0;

    virtual void broadcast() = 0;
};

} // namespace Karm::Sys
