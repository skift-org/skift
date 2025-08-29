#pragma once

#include <hjert-api/api.h>
#include <impl-skift/fd.h>
#include <karm-logger/logger.h>
#include <karm-mime/url.h>
#include <karm-sys/endpoint.h>
#include <karm-sys/context.h>

namespace Strata::Bus {

struct Bus;

struct Endpoint : public Meta::Pinned {
    static Sys::Port nextPort() {
        static usize port = 2;
        return Sys::Port{port++};
    }

    Sys::Port _port = nextPort();
    Bus* _bus;

    virtual ~Endpoint() = default;

    Sys::Port port() const { return _port; }

    void attach(Bus& bus) { _bus = &bus; }

    Res<> dispatch(Sys::Message& msg);

    virtual Str id() const = 0;

    virtual Res<> send(Sys::Message&) { return Ok(); }

    virtual bool accept(Sys::Message const&) { return true; }

    virtual Res<> activate(Sys::Context&) { return Ok(); }
};

struct Service : public Endpoint {
    String _id;
    Vec<Meta::Id> _listen;
    Rc<Skift::IpcFd> _ipc;
    Sys::IpcConnection _con;
    Opt<Hj::Task> _task = NONE;

    static Res<Rc<Service>> prepare(Sys::Context& ctx, Str id);

    Service(Str id, Rc<Skift::IpcFd> ipc)
        : _id{id}, _ipc{ipc}, _con{ipc, ""_url} {
    }

    Str id() const override { return _id; }

    Res<> activate(Sys::Context& ctx) override;

    Async::Task<> runAsync();

    Res<> send(Sys::Message& msg) override;

    bool accept(Sys::Message const& msg) override;
};

struct System : public Endpoint {
    System();

    Str id() const override;

    Res<> send(Sys::Message& msg) override;
};

struct Bus : public Meta::Pinned {
    Sys::Context& _context;

    Vec<Rc<Endpoint>> _endpoints{};

    Bus(Sys::Context& ctx)
        : _context(ctx) {}

    static Res<Rc<Bus>> create(Sys::Context& ctx);

    Res<> attach(Rc<Endpoint> endpoint);

    void _broadcast(Sys::Message& msg);

    Res<> dispatch(Sys::Message& msg);

    Res<> prepareService(Str id);

    Res<> prepareActivateService(Str id);
};

} // namespace Strata::Bus
