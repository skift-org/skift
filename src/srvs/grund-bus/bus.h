#pragma once

#include <hjert-api/api.h>
#include <impl-skift/fd.h>
#include <karm-logger/logger.h>
#include <karm-mime/url.h>
#include <karm-sys/context.h>
#include <karm-sys/ipc.h>

namespace Grund::Bus {

struct Bus;

struct Endpoint : public Meta::Static {
    static Sys::Port nextPort() {
        static usize port = 2;
        return Sys::Port{port++};
    }

    Sys::Port _port;
    Bus *_bus;

    virtual ~Endpoint() = default;

    Sys::Port port() const { return _port; }

    void attach(Bus &bus) { _bus = &bus; }

    virtual Res<> dispatch(Sys::Message &) { return Ok(); }

    virtual Res<> activate(Sys::Context &) { return Ok(); }
};

struct Service : public Endpoint {
    String _id;
    Strong<Skift::IpcFd> _ipc;
    Sys::IpcConnection _con;
    Opt<Hj::Task> _task = NONE;

    static Res<Strong<Service>> prepare(Sys::Context &ctx, Str id);

    Service(Str id, Strong<Skift::IpcFd> ipc)
        : _id{id}, _ipc{ipc}, _con{ipc, ""_url} {
    }

    Res<> activate(Sys::Context &ctx) override;

    Async::Task<> runAsync();

    Res<> dispatch(Sys::Message &msg) override;
};

struct Bus : public Meta::Static {
    Sys::Context &_context;

    Vec<Strong<Endpoint>> _endpoints{};

    Bus(Sys::Context &ctx)
        : _context(ctx) {}

    static Res<Strong<Bus>> create(Sys::Context &ctx);

    Res<> attach(Strong<Endpoint> endpoint);

    Res<> dispatch(Sys::Header &h, Sys::Message &msg);

    Res<> startService(Str id);
};

} // namespace Grund::Bus
