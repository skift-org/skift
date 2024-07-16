#pragma once

#include <hjert-api/api.h>
#include <karm-logger/logger.h>
#include <karm-mime/url.h>
#include <karm-sys/context.h>

namespace Grund::System {

struct Service {
    String _id;
    Hj::Channel _in;
    Hj::Channel _out;
    Opt<Hj::Task> _task = NONE;

    static Res<Strong<Service>> prepare(Sys::Context &ctx, Str id);

    Res<> activate(Sys::Context &ctx);
};

struct System {
    Sys::Context &_context;
    Hj::Listener _listener;
    Hj::Domain _domain;

    Vec<Strong<Service>> _services{};

    static Res<System> create(Sys::Context &ctx) {
        auto domain = try$(Hj::Domain::create(Hj::ROOT));
        auto listener = try$(Hj::Listener::create(Hj::ROOT));
        return Ok(System{ctx, std::move(listener), std::move(domain)});
    }

    Res<> prepare(Str id) {
        auto service = try$(Service::prepare(_context, id));
        try$(_attach(service));
        return Ok();
    }

    Res<> _attach(Strong<Service> service) {
        try$(_listener.listen(service->_out, Hj::Sigs::READABLE, Hj::Sigs::NONE));
        _services.pushBack(std::move(service));
        return Ok();
    }

    Res<> run() {
        for (auto &service : _services) {
            try$(service->activate(_context));
        }

        logDebug("running system event loop");
        while (true) {
            try$(_listener.poll(TimeStamp::endOfTime()));
            while (auto ev = _listener.next()) {
                logInfo("handling system event");
                try$(Hj::_signal(ev->cap, Hj::Sigs::NONE, Hj::Sigs::READABLE));
            }
        }
    }
};

} // namespace Grund::System
