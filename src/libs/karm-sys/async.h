#pragma once

#include <karm-base/async.h>

#include "fd.h"

namespace Karm::Sys {

struct Sched :
    Meta::Pinned {

    Opt<Res<>> _ret;

    virtual ~Sched() = default;

    bool exited() const { return _ret.has(); }

    void quit(Res<> ret) { _ret = ret; }

    virtual Res<> wait(TimeStamp until) = 0;

    virtual Async::Task<usize> readAsync(Strong<Fd>, MutBytes) = 0;

    virtual Async::Task<usize> writeAsync(Strong<Fd>, Bytes) = 0;

    virtual Async::Task<usize> flushAsync(Strong<Fd>) = 0;

    virtual Async::Task<_Accepted> acceptAsync(Strong<Fd>) = 0;

    virtual Async::Task<_Sent> sendAsync(Strong<Fd>, Bytes, Slice<Handle>, SocketAddr) = 0;

    virtual Async::Task<_Received> recvAsync(Strong<Fd>, MutBytes, MutSlice<Handle>) = 0;

    virtual Async::Task<> sleepAsync(TimeStamp until) = 0;
};

Sched &globalSched();

template <Async::Sender S>
auto run(S s, Sched &sched = globalSched()) {
    return Async::run(std::move(s), [&] {
        (void)sched.wait(TimeStamp::endOfTime());
    });
}

} // namespace Karm::Sys
