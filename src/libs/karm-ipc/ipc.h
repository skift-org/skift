#pragma once

#include <karm-logger/logger.h>
#include <karm-sys/context.h>
#include <karm-sys/socket.h>

#include "base.h"
#include "hook.h"

namespace Karm::Ipc {

struct _Object;

struct _Pending {
    u64 seq;
    virtual ~_Pending() = default;
    virtual void complete(Io::PackScan &) = 0;
};

template <typename T>
struct Pending : public _Pending {
    Async::_Promise<T> promise;
    void complete(Io::PackScan &s) override {
        auto res = Io::unpack<T>(s);
        if (not res) {
            promise.resolve(res.none());
        } else {
            promise.resolve(res.take());
        }
    }
};

struct Server {
    Sys::IpcConnection _con;
    Map<u64, _Object *> _objects;
    Map<u64, _Pending *> _pending;

    Server(Sys::IpcConnection con)
        : _con(std::move(con)) {}

    static Res<Server> create(Sys::Context &ctx);

    void attach(_Object &obj);

    void detach(_Object &obj);

    void attach(_Pending &p);

    void detach(_Pending &p);

    Async::Task<> runAsync();
};

// MARK: Server ----------------------------------------------------------------

struct _Object : public Meta::Static {
    u64 _oid;
    Server &_server;

    _Object(Server &server);

    ~_Object();

    virtual Async::Task<> handleRequest(Header header, Io::PackScan &req, Io::PackEmit &res) = 0;
};

struct Dispatch {
    Header header;
    Io::PackScan &req;
    Io::PackEmit &res;

    auto mid() const {
        return header.mid;
    }

    template <typename... Args>
    Async::Task<> call(auto &&f) {
        auto args = co_try$(Io::unpack<Tuple<Args...>>(req));
        auto res = co_trya$(args.apply(f));
        co_return Ok();
    }

    Async::Task<> error() {
        co_return Error::invalidData("unknown method id");
    }
};

template <typename I>
struct Object : public I, public _Object {
    using _Object::_Object;
    using I::_dispatch;

    Async::Task<> handleRequest(Header header, Io::PackScan &req, Io::PackEmit &resp) override {
        Dispatch d{header, req, resp};
        return I::_dispatch(d);
    }
};

// MARK: Client ----------------------------------------------------------------

struct Transport {
    usize _oid;
    Server &_server;

    static usize _seq;

    template <typename I, u64 MID, typename Ret, typename... Args>
    Ret invoke(Args &&...args) {
        Header header{
            .from = 0,
            .to = 0,
            .oid = _oid,
            .uid = I::_UID,
            .mid = MID,
            .seq = _seq++,
        };

        Tuple<Args...> params{std::forward<Args>(args)...};

        Io::BufferWriter reqBuf;
        Io::PackEmit reqPack{reqBuf};

        co_try$(Io::pack(reqPack, header));
        co_try$(Io::pack(reqPack, params));

        co_trya$(_server._con.sendAsync(reqBuf.bytes(), reqPack.handles()));
        Pending<typename Ret::Inner> pending;
        _server.attach(pending);
        co_return co_trya$(pending.promise.future());
    }
};

template <typename I>
Strong<I> open(Server &srv, u64 oid) {
    return makeStrong<typename I::template _Client<Transport>>(Transport{oid, srv});
}

} // namespace Karm::Ipc
