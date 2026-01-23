
module Karm.Sys;

import Karm.Core;

namespace Karm::Sys::_Embed {

struct EfiSched : Sched {
    Res<> wait(Instant) override {
        while (true)
            ;
    }

    Async::Task<usize> readAsync(Rc<Fd> fd, MutBytes buf, Async::CancellationToken) override {
        co_return fd->read(buf);
    }

    Async::Task<usize> writeAsync(Rc<Fd> fd, Bytes buf, Async::CancellationToken) override {
        co_return fd->write(buf);
    }

    Async::Task<> flushAsync(Rc<Fd> fd, Async::CancellationToken) override {
        co_return fd->flush();
    }

    Async::Task<_Accepted> acceptAsync(Rc<Fd>, Async::CancellationToken) override {
        co_return Error::notImplemented();
    }

    Async::Task<_Sent> sendAsync(Rc<Fd>, Bytes, Slice<Handle>, SocketAddr, Async::CancellationToken) override {
        co_return Error::notImplemented();
    }

    Async::Task<_Received> recvAsync(Rc<Fd>, MutBytes, MutSlice<Handle>, Async::CancellationToken) override {
        co_return Error::notImplemented();
    }

    Async::Task<Flags<Poll>> pollAsync(Rc<Fd>, Flags<Poll>, Async::CancellationToken) override {
        co_return Error::notImplemented();
    }

    Async::Task<> sleepAsync(Instant, Async::CancellationToken) override {
        co_return Error::notImplemented();
    }
};

Sched& globalSched() {
    static EfiSched sched = [] -> EfiSched {
        return {};
    }();
    return sched;
}

} // namespace Karm::Sys::_Embed
