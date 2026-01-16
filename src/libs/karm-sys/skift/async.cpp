module;

#include <hjert-api/api.h>

module Karm.Sys;

import Karm.Core;
import Karm.Logger;
import Karm.Sys.Skift;

namespace Karm::Sys::_Embed {

struct SkiftSched : Sys::Sched {
    Hj::Listener _listener;
    Map<Hj::Cap, Async::Promise<>> _promises;
    Vec<Pair<Instant, Async::Promise<>>> _sleeps;

    SkiftSched(Hj::Listener listener) : _listener{std::move(listener)} {}

    Async::Task<> waitFor(Hj::Cap cap, Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset) {
        if (_promises.has(cap))
            // FIXME: We only support one waiter per cap
            panic("already waiting for this cap");

        co_try$(_listener.listen(cap, set, unset));
        auto promise = Async::Promise<>();
        auto future = promise.future();

        _promises.put(cap, std::move(promise));

        co_return co_await future;
    }

    Async::Task<usize> readAsync(Rc<Fd>, MutBytes, Async::CancellationToken) override {
        co_return Error::notImplemented("not implemented");
    }

    Async::Task<usize> writeAsync(Rc<Fd>, Bytes, Async::CancellationToken) override {
        co_return Error::notImplemented("not implemented");
    }

    Async::Task<> flushAsync(Rc<Fd>, Async::CancellationToken) override {
        co_return Error::notImplemented("not implemented");
    }

    Async::Task<_Accepted> acceptAsync(Rc<Fd>, Async::CancellationToken) override {
        co_return Error::notImplemented("not implemented");
    }

    Async::Task<_Sent> sendAsync(Rc<Fd> fd, Bytes buf, Slice<Handle> hnds, SocketAddr, Async::CancellationToken) override {
        if (auto ipc = fd.is<Skift::IpcFd>()) {
            auto& chan = ipc->_out;

            co_trya$(waitFor(chan.cap(), Hj::Sigs::WRITABLE, Hj::Sigs::NONE));
            static_assert(sizeof(Handle) == sizeof(Hj::Cap) and alignof(Handle) == alignof(Hj::Cap));
            co_try$(chan.send(buf, hnds.cast<Hj::Cap>()));

            co_return Ok<_Sent>({buf.len(), hnds.len()});
        }

        co_return Error::notImplemented("unsupported fd type");
    }

    Async::Task<_Received> recvAsync(Rc<Fd> fd, MutBytes buf, MutSlice<Handle> hnds, Async::CancellationToken) override {
        if (auto ipc = fd.is<Skift::IpcFd>()) {
            auto& chan = ipc->_in;

            co_trya$(waitFor(chan.cap(), Hj::Sigs::READABLE, Hj::Sigs::NONE));
            static_assert(sizeof(Handle) == sizeof(Hj::Cap) and alignof(Handle) == alignof(Hj::Cap));
            auto [len, hndsLen] = co_try$(chan.recv(buf, hnds.cast<Hj::Cap>()));

            co_return Ok<_Received>{len, hndsLen, Sys::Ip4::unspecified(0)};
        }

        co_return Error::notImplemented("unsupported fd type");
    }

    Async::Task<Flags<Poll>> pollAsync(Rc<Fd>, Flags<Poll>, Async::CancellationToken) override {
        co_return Error::notImplemented();
    }

    Async::Task<> sleepAsync(Instant stamp, Async::CancellationToken) override {
        Async::Promise<> promise;
        auto future = promise.future();
        _sleeps.pushBack({stamp, std::move(promise)});
        co_return co_await future;
    }

    Instant _soonest() {
        auto soonest = Instant::endOfTime();

        for (auto const& [stamp, _] : _sleeps) {
            if (stamp < soonest)
                soonest = stamp;
        }

        return soonest;
    }

    void _wake(Instant now) {
        for (usize i = 0; i < _sleeps.len(); i++) {
            auto& [stamp, promise] = _sleeps[i];
            if (stamp <= now) {
                promise.resolve(Ok());
                _sleeps.removeAt(i);
                i--;
            }
        }
    }

    Res<> wait(Instant until) override {
        for (;;) {
            auto now = _Embed::instant();
            _wake(now);

            if (now >= until)
                return Ok();

            auto nextDeadline = min(until, _soonest());
            if (nextDeadline <= _Embed::instant())
                continue;

            try$(_listener.poll(nextDeadline));

            while (auto ev = _listener.next()) {
                auto prop = _promises.take(ev->cap);
                try$(_listener.mute(ev->cap));
                prop.resolve(Ok());
                if (_Embed::instant() >= until)
                    return Ok();
            }
        }
    }
};

Sched& globalSched() {
    static SkiftSched sched = [] -> SkiftSched {
        return {
            Hj::Listener::create(Hj::ROOT)
                .take("failed to create listener"),
        };
    }();
    return sched;
}

} // namespace Karm::Sys::_Embed
