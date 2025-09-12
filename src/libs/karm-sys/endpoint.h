#pragma once

#include "context.h"
#include "message.h"
#include "socket.h"

namespace Karm::Sys {

// MARK: Primitive Operations --------------------------------------------------

template <typename T, typename... Args>
Res<> rpcSend(Sys::IpcConnection& con, Port to, u64 seq, Args&&... args) {
    Message msg = Message::packReq<T>(to, seq, std::forward<Args>(args)...).take();

    try$(con.send(msg.bytes(), msg.handles()));
    return Ok();
}

static inline Async::Task<Message> rpcRecvAsync(Sys::IpcConnection& con) {
    Message msg;
    auto [bufLen, hndsLen] = co_trya$(con.recvAsync(msg._buf, msg._hnds));
    if (bufLen < sizeof(Header))
        co_return Error::invalidData("invalid message");
    msg._len = bufLen;
    msg._hndsLen = hndsLen;

    co_return msg;
}

// MARK: Rpc -------------------------------------------------------------------

struct Endpoint : Meta::Pinned {
    Sys::IpcConnection _con;
    Map<u64, Async::_Promise<Message>> _pending{};
    Async::Queue<Message> _incoming{};
    u64 _seq = 1;

    static Res<Endpoint> use(Context& ctx) {}

    Endpoint(Sys::IpcConnection con);

    static Endpoint create(Sys::Context& ctx);

    static Async::Task<> _receiverTask(Endpoint& self) {
        while (true) {
            Message msg = co_trya$(rpcRecvAsync(self._con));
            auto header = msg._header;

            if (self._pending.has(header.seq)) {
                auto promise = self._pending.take(header.seq);
                promise.resolve(std::move(msg));
            } else {
                self._incoming.enqueue(std::move(msg));
            }
        }
    }

    template <typename T, typename... Args>
    Res<> send(Port port, Args&&... args) {
        return rpcSend<T>(_con, port, _seq++, std::forward<Args>(args)...);
    }

    Async::Task<Message> recvAsync() {
        co_return Ok(co_await _incoming.dequeueAsync());
    }

    template <typename T>
    Res<> resp(Message& msg, Res<typename T::Response> message) {
        auto header = msg._header;
        if (not message)
            return rpcSend<Error>(_con, header.from, header.seq, message.none());
        return rpcSend<typename T::Response>(_con, header.from, header.seq, message.take());
    }

    template <typename T, typename... Args>
    Async::Task<typename T::Response> callAsync(Port port, Args&&... args) {
        auto seq = _seq++;
        Async::_Promise<Message> promise;
        auto future = promise.future();
        _pending.put(seq, std::move(promise));

        co_try$(rpcSend<T>(_con, port, seq, std::forward<Args>(args)...));

        Message msg = co_await future;

        if (msg.is<Error>())
            co_return co_try$(msg.unpack<Error>());

        if (not msg.is<typename T::Response>())
            co_return Error::invalidInput("unexpected response");

        co_return Ok(co_try$(msg.unpack<typename T::Response>()));
    }
};

} // namespace Karm::Sys