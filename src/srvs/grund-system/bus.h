#pragma once

#include <hjert-api/api.h>
#include <karm-main/base.h>
#include <url/url.h>

#include "api.h"

namespace Grund::System {

struct Unit {
    Hj::Task _task;
    Hj::Channel _in;
    Hj::Channel _out;

    static Res<Strong<Unit>> load(Ctx &ctx, Url::Url url);
};

struct Object {
    usize _id;
    usize _port;
    Strong<Unit> _unit;

    static Res<Strong<Object>> create(usize port, Strong<Unit> unit) {
        static usize id = 0;
        return Ok(makeStrong<Object>(id++, port, unit));
    }
};

struct Bus {
    Hj::Listener _listener;
    Hj::Domain _domain;

    Vec<Strong<Unit>> _units{};
    Vec<Strong<Object>> _objs{};

    static Res<Bus> create() {
        auto domain = try$(Hj::Domain::create(Hj::ROOT));
        auto listener = try$(Hj::Listener::create(Hj::ROOT));
        return Ok(Bus{std::move(listener), std::move(domain)});
    }

    Res<> attach(Strong<Unit> unit) {
        try$(_listener.listen(unit->_out, Hj::Sigs::READABLE, Hj::Sigs::NONE));
        _units.pushBack(std::move(unit));
        return Ok();
    }

    Res<> broadcast(Hj::Cap from, Hj::Msg const &msg) {
        for (auto &unit : _units)
            if (unit->_out.cap() != from)
                try$(unit->_in.send(msg, _domain));

        return Ok();
    }

    Res<> run() {
        while (true) {
            try$(_listener.poll(TimeStamp::endOfTime()));
            auto ev = _listener.next();
            while (ev) {
                try$(Hj::_signal(ev->cap, Hj::Sigs::NONE, Hj::Sigs::READABLE));
                Hj::Msg msg;
                try$(Hj::_recv(ev->cap, &msg, _domain));
                if (msg.label == IBus::broadcast_UID) {
                    try$(broadcast(ev->cap, msg));
                }
                ev = _listener.next();
            }
        }
    }
};

} // namespace Grund::System
