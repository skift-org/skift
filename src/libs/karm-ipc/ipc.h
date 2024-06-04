#pragma once

#include <karm-logger/logger.h>
#include <karm-sys/context.h>
#include <karm-sys/socket.h>

#include "hook.h"

namespace Karm::Ipc {

struct _Object;

struct Header {
    u64 from, oid, uid, mid, seq;
};

struct Server {
    Sys::IpcConnection _con;
    Map<u64, _Object *> _objects;

    Server(Sys::IpcConnection con)
        : _con(std::move(con)) {}

    static Res<Server> create(Sys::Ctx &ctx);

    void attach(_Object &obj);

    void detach(_Object &obj);

    Async::Task<> runAsync();
};

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

} // namespace Karm::Ipc
