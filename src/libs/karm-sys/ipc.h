#pragma once

#include <karm-base/cons.h>
#include <karm-base/map.h>
#include <karm-io/pack.h>
#include <karm-logger/logger.h>
#include <karm-sys/async.h>
#include <karm-sys/context.h>
#include <karm-sys/socket.h>

struct ChannelHook : public Sys::Service {
    Sys::IpcConnection con;

    ChannelHook(Sys::IpcConnection con)
        : con(std::move(con)) {}
};

inline ChannelHook &useChannel(Sys::Context &ctx = Sys::globalContext()) {
    return ctx.use<ChannelHook>();
}

namespace Karm::Sys {

// MARK: Primitive Types -------------------------------------------------------

struct Port : public Distinct<u64, struct _PortTag> {
    static Port const INVALID;
    static Port const BUS;

    using Distinct::Distinct;

    void repr(Io::Emit &e) const {
        e("{}", value());
    }
};

constexpr Port Port::INVALID{0};
constexpr Port Port::BUS{1};

struct Header {
    u64 seq;
    Port port;
    Meta::Id mid;

    void repr(Io::Emit &e) const {
        e("(header seq: {}, port: {}, mid: {:016x})", seq, port, mid);
    }
};

struct Message {
    Array<u8, 4096> bytes;
    usize len;
    Array<Sys::Handle, 16> handles;
    usize handlesLen;

    Res<Header> header() const {
        Io::PackScan s{bytes, handles};
        return Io::unpack<Header>(s);
    }

    template <typename T>
    bool is() const {
        auto maybeHeader = header();
        if (not maybeHeader)
            return false;
        return maybeHeader.unwrap().mid == Meta::idOf<T>();
    }

    template <typename T>
    Res<T> unpack() {
        Io::PackScan s{bytes, handles};
        if (not is<T>())
            return Error::invalidData("unexpected message");
        try$(Io::unpack<Header>(s));
        return Io::unpack<T>(s);
    }
};

// MARK: Primitive Operations --------------------------------------------------

template <typename T, typename... Args>
Res<> ipcSend(Sys::IpcConnection &con, Port port, u64 seq, Args &&...args) {
    Header header{seq, port, Meta::idOf<T>()};
    T msg{std::forward<Args>(args)...};

    Io::BufferWriter reqBuf;
    Io::PackEmit reqPack{reqBuf};

    try$(Io::pack(reqPack, header));
    try$(Io::pack(reqPack, msg));

    try$(con.send(reqBuf.bytes(), reqPack.handles()));

    return Ok();
}

Async::Task<Message> ipcRecvAsync(Sys::IpcConnection &con);

// MARK: Ipc -------------------------------------------------------------------

struct Ipc {
    Sys::IpcConnection _con;
    bool _receiving = false;
    Map<u64, Async::_Promise<Message>> _pending{};
    u64 _seq = 1;

    static Ipc create(Sys::Context &ctx) {
        auto &channel = useChannel(ctx);
        return Ipc{std::move(channel.con)};
    }

    template <typename T, typename... Args>
    Res<> send(Port port, Args &&...args) {
        return ipcSend<T>(_con, port, _seq++, std::forward<Args>(args)...);
    }

    Async::Task<Message> recvAsync() {
        if (_receiving)
            co_return Error::other("already receiving");

        _receiving = true;
        Defer defer{[this] {
            _receiving = false;
        }};

        while (true) {
            Message msg = co_trya$(ipcRecvAsync(_con));

            auto maybeHeader = msg.header();
            if (not maybeHeader) {
                logWarn("dropping message: {}", maybeHeader.none().msg());
                continue;
            }

            if (_pending.has(maybeHeader.unwrap().seq)) {
                auto promise = _pending.take(maybeHeader.unwrap().seq);
                promise.resolve(msg);
                continue;
            }

            co_return msg;
        }
    }

    template <typename T>
    Res<> resp(Message &msg, Res<typename T::Response> message) {
        auto header = try$(msg.header());
        if (not message)
            return ipcSend<Error>(_con, header.port, header.seq, message.none());

        return ipcSend<typename T::Response>(_con, header.port, header.seq, message.take());
    }

    template <typename T, typename... Args>
    Async::Task<typename T::Response> callAsync(Port port, Args &&...args) {
        auto seq = _seq++;
        Async::_Promise<Message> promise;
        auto future = promise.future();

        _pending.put(seq, std::move(promise));

        co_try$(ipcSend<T>(_con, port, seq, std::forward<Args>(args)...));

        Message msg = co_await future;

        if (msg.is<Error>())
            co_return co_try$(msg.unpack<Error>());

        if (not msg.is<typename T::Response>())
            co_return Error::invalidInput("unexpected response");

        co_return Ok(co_try$(msg.unpack<typename T::Response>()));
    }
};

} // namespace Karm::Sys
