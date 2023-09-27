#include "channel.h"

#include "domain.h"

namespace Hjert::Core {

Res<Parcel> Parcel::fromMsg(Domain &dom, Hj::Msg msg) {
    Parcel parcel;
    parcel._label = msg.label;
    parcel._flags = msg.flags;

    for (usize i = 0; i < 6; i++) {
        if (msg.isCap(i)) {
            parcel._slots[i] = try$(dom.get(try$(msg.loadCap(i))));
        } else {
            parcel._slots[i] = try$(msg.loadArg(i));
        }
    }

    return Ok(Parcel{});
}

Res<Hj::Msg> Parcel::toMsg(Domain &dom) {
    Hj::Msg msg{_label};

    for (usize i = 0; i < 6; i++) {
        try$(_slots[i].visit(
            Visitor{
                [&](None) -> Res<> {
                    return Ok();
                },
                [&](Hj::Arg arg) -> Res<> {
                    msg.storeArg(i, arg);
                    return Ok();
                },
                [&](Strong<Object> obj) -> Res<> {
                    msg.storeCap(i, try$(dom.add(Hj::ROOT, obj)));
                    return Ok();
                },
            }));
    }

    return Ok(msg);
}

Res<Strong<Channel>> Channel::create(usize cap) {
    return Ok(makeStrong<Channel>(cap));
}

Channel::Channel(usize cap) {
    _cap = cap;
}

Res<> Channel::_ensureNoEmpty() {
    if (_ring.len() == 0) {
        return Error::wouldBlock("channel empty");
    }
    return Ok();
}

Res<> Channel::_ensureNoFull() {
    if (_ring.len() == _cap) {
        return Error::wouldBlock("channel full");
    }
    return Ok();
}

Res<> Channel::_ensureOpen() {
    if (_closed) {
        return Error::brokenPipe("channel closed");
    }
    return Ok();
}

void Channel::_updateSignalsUnlock() {
    _signalUnlock(
        _closed ? Hj::Sigs::CLOSED : Hj::Sigs::NONE,
        _closed ? Hj::Sigs::NONE : Hj::Sigs::CLOSED);
}

Res<> Channel::send(Domain &dom, Hj::Msg msg) {
    ObjectLockScope scope{*this};
    try$(_ensureOpen());
    try$(_ensureNoFull());
    _ring.pushBack(try$(Parcel::fromMsg(dom, msg)));
    return Ok();
}

Res<Hj::Msg> Channel::recv(Domain &dom) {
    ObjectLockScope scope{*this};
    try$(_ensureOpen());
    try$(_ensureNoEmpty());
    auto parcel = _ring.popBack();
    return parcel.toMsg(dom);
}

Res<> Channel::close() {
    ObjectLockScope scope{*this};
    _closed = true;
    return Ok();
}

} // namespace Hjert::Core
