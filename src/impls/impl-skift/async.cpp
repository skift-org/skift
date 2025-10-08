module;

#include <hjert-api/api.h>

#include "fd.h"

module Karm.Sys;

import Karm.Core;
import Karm.Logger;

namespace Karm::Sys::_Embed {

struct HjertSched : Sys::Sched {
    Hj::Listener _listener;
    Map<Hj::Cap, Async::Promise<>> _promises;
    Vec<Pair<Instant, Async::Promise<>>> _sleeps;

    HjertSched(Hj::Listener listener) : _listener{std::move(listener)} {}

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

    virtual Async::Task<usize> readAsync(Rc<Fd>, MutBytes) {
        co_return Error::notImplemented("not implemented");
    }

    virtual Async::Task<usize> writeAsync(Rc<Fd>, Bytes) {
        co_return Error::notImplemented("not implemented");
    }

    virtual Async::Task<> flushAsync(Rc<Fd>) {
        co_return Error::notImplemented("not implemented");
    }

    virtual Async::Task<_Accepted> acceptAsync(Rc<Fd>) {
        co_return Error::notImplemented("not implemented");
    }

    virtual Async::Task<_Sent> sendAsync(Rc<Fd> fd, Bytes buf, Slice<Handle> hnds, SocketAddr) {
        if (auto ipc = fd.is<Skift::IpcFd>()) {
            auto& chan = ipc->_out;

            co_trya$(waitFor(chan.cap(), Hj::Sigs::WRITABLE, Hj::Sigs::NONE));
            static_assert(sizeof(Handle) == sizeof(Hj::Cap) and alignof(Handle) == alignof(Hj::Cap));
            co_try$(chan.send(buf, hnds.cast<Hj::Cap>()));

            co_return Ok<_Sent>({buf.len(), hnds.len()});
        }

        co_return Error::notImplemented("unsupported fd type");
    }

    virtual Async::Task<_Received> recvAsync(Rc<Fd> fd, MutBytes buf, MutSlice<Handle> hnds) {
        if (auto ipc = fd.is<Skift::IpcFd>()) {
            auto& chan = ipc->_in;

            co_trya$(waitFor(chan.cap(), Hj::Sigs::READABLE, Hj::Sigs::NONE));
            static_assert(sizeof(Handle) == sizeof(Hj::Cap) and alignof(Handle) == alignof(Hj::Cap));
            auto [len, hndsLen] = co_try$(chan.recv(buf, hnds.cast<Hj::Cap>()));

            co_return Ok<_Received>{len, hndsLen, Sys::Ip4::unspecified(0)};
        }

        co_return Error::notImplemented("unsupported fd type");
    }

    virtual Async::Task<> sleepAsync(Instant stamp) {
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

    virtual Res<> wait(Instant until) {
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
    static HjertSched sched = [] -> HjertSched {
        return {
            Hj::Listener::create(Hj::ROOT)
                .take("failed to create listener"),
        };
    }();
    return sched;
}

} // namespace Karm::Sys::_Embed
