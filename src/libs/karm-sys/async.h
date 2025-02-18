#pragma once

#include <karm-async/run.h>
#include <karm-async/task.h>

#include "fd.h"

namespace Karm::Sys {

struct Sched :
    Meta::Pinned {

    Opt<Res<>> _ret;

    virtual ~Sched() = default;

    bool exited() const { return _ret.has(); }

    void quit(Res<> ret) { _ret = ret; }

    virtual Res<> wait(Instant until) = 0;

    virtual Async::Task<usize> readAsync(Rc<Fd>, MutBytes) = 0;

    virtual Async::Task<usize> writeAsync(Rc<Fd>, Bytes) = 0;

    virtual Async::Task<> flushAsync(Rc<Fd>) = 0;

    virtual Async::Task<_Accepted> acceptAsync(Rc<Fd>) = 0;

    virtual Async::Task<_Sent> sendAsync(Rc<Fd>, Bytes, Slice<Handle>, SocketAddr) = 0;

    virtual Async::Task<_Received> recvAsync(Rc<Fd>, MutBytes, MutSlice<Handle>) = 0;

    virtual Async::Task<> sleepAsync(Instant until) = 0;
};

Sched& globalSched();

template <Async::Sender S>
auto run(S s, Sched& sched = globalSched()) {
    return Async::run(std::move(s), [&] {
        (void)sched.wait(Instant::endOfTime());
    });
}

} // namespace Karm::Sys
