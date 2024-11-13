#include <hjert-api/api.h>
#include <karm-base/map.h>
#include <karm-logger/logger.h>

#include <karm-sys/_embed.h>

#include "fd.h"

namespace Karm::Sys::_Embed {

struct HjertSched : public Sys::Sched {
    Hj::Listener _listener;
    Map<Hj::Cap, Async::Promise<>> _promises;

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

    virtual Async::Task<usize> readAsync(Strong<Fd>, MutBytes) {
        co_return Error::notImplemented("not implemented");
    }

    virtual Async::Task<usize> writeAsync(Strong<Fd>, Bytes) {
        co_return Error::notImplemented("not implemented");
    }

    virtual Async::Task<usize> flushAsync(Strong<Fd>) {
        co_return Error::notImplemented("not implemented");
    }

    virtual Async::Task<_Accepted> acceptAsync(Strong<Fd>) {
        co_return Error::notImplemented("not implemented");
    }

    virtual Async::Task<_Sent> sendAsync(Strong<Fd> fd, Bytes buf, Slice<Handle> hnds, SocketAddr) {
        if (auto ipc = fd.is<Skift::IpcFd>()) {
            auto &chan = ipc->_out;

            co_trya$(waitFor(chan.cap(), Hj::Sigs::WRITABLE, Hj::Sigs::NONE));
            static_assert(sizeof(Handle) == sizeof(Hj::Cap) and alignof(Handle) == alignof(Hj::Cap));
            logDebug("sending len: {}, hndsLen: {}", buf.len(), hnds.len());
            co_try$(chan.send(buf, hnds.cast<Hj::Cap>()));

            co_return Ok<_Sent>({buf.len(), hnds.len()});
        }

        co_return Error::notImplemented("unsupported fd type");
    }

    virtual Async::Task<_Received> recvAsync(Strong<Fd> fd, MutBytes buf, MutSlice<Handle> hnds) {
        if (auto ipc = fd.is<Skift::IpcFd>()) {
            auto &chan = ipc->_in;

            logDebug("Buffer size: {}", buf.len());

            co_trya$(waitFor(chan.cap(), Hj::Sigs::READABLE, Hj::Sigs::NONE));
            static_assert(sizeof(Handle) == sizeof(Hj::Cap) and alignof(Handle) == alignof(Hj::Cap));
            auto [len, hndsLen] = co_try$(chan.recv(buf, hnds.cast<Hj::Cap>()));
            logDebug("got len: {}, hndsLen: {}", len, hndsLen);

            co_return Ok<_Received>{buf.len(), hnds.len(), Sys::Ip4::unspecified(0)};
        }

        co_return Error::notImplemented("unsupported fd type");
    }

    virtual Async::Task<> sleepAsync(TimeStamp) {
        co_return Error::notImplemented("not implemented");
    }

    virtual Res<> wait(TimeStamp until) {
        while (_Embed::now() < until) {
            try$(_listener.poll(until));
            while (auto ev = _listener.next()) {
                auto prop = _promises.take(ev->cap);
                try$(_listener.mute(ev->cap));
                prop.resolve(Ok());
            }
        }
        return Ok();
    }
};

Sched &globalSched() {
    static HjertSched sched = [] -> HjertSched {
        return {
            Hj::Listener::create(Hj::ROOT)
                .take("failed to create listener"),
        };
    }();
    return sched;
}

} // namespace Karm::Sys::_Embed
