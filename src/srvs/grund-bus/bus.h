#pragma once

#include <hjert-api/api.h>
#include <impl-skift/fd.h>
#include <karm-logger/logger.h>
#include <karm-mime/url.h>
#include <karm-sys/context.h>
#include <karm-sys/rpc.h>

namespace Grund::Bus {

struct Bus;

struct Endpoint : public Meta::Pinned {
    static Sys::Port nextPort() {
        static usize port = 2;
        return Sys::Port{port++};
    }

    Sys::Port _port = nextPort();
    Bus *_bus;

    virtual ~Endpoint() = default;

    Sys::Port port() const { return _port; }

    void attach(Bus &bus) { _bus = &bus; }

    Res<> dispatch(Sys::Message &msg);

    virtual Str id() const = 0;

    virtual Res<> send(Sys::Message &) { return Ok(); }

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

    Str id() const override { return _id; }

    Res<> activate(Sys::Context &ctx) override;

    Async::Task<> runAsync();

    Res<> send(Sys::Message &msg) override;
};

struct Locator : public Endpoint {
    Locator();

    Str id() const override;

    Res<> send(Sys::Message &msg) override;
};

struct Bus : public Meta::Pinned {
    Sys::Context &_context;

    Vec<Strong<Endpoint>> _endpoints{};

    Bus(Sys::Context &ctx)
        : _context(ctx) {}

    static Res<Strong<Bus>> create(Sys::Context &ctx);

    Res<> attach(Strong<Endpoint> endpoint);

    void _broadcast(Sys::Message &msg);

    Res<> dispatch(Sys::Message &msg);

    Res<> startService(Str id);
};

} // namespace Grund::Bus
