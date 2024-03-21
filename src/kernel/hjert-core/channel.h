#pragma once

#include <karm-base/ring.h>

#include "object.h"

namespace Hjert::Core {

struct Domain;

struct Parcel {
    using Slot = Union<None, Hj::Arg, Strong<Object>>;

    Hj::Arg _label;
    Hj::Arg _flags;
    Array<Slot, 6> _slots = makeArray<Slot, 6>(NONE);

    static Res<Parcel> fromMsg(Domain &dom, Hj::Msg msg);
    Res<Hj::Msg> toMsg(Domain &dom);
};

struct Channel :
    public BaseObject<Channel, Hj::Type::CHANNEL> {

    bool _closed = false;
    Ring<Parcel> _ring;
    usize _cap;

    static Res<Strong<Channel>> create(usize cap = 4096);

    Channel(usize cap);

    Res<> _ensureNoEmpty();

    Res<> _ensureNoFull();

    Res<> _ensureOpen();

    void _updateSignalsUnlock();

    Res<> send(Domain &dom, Hj::Msg msg);

    Res<Hj::Msg> recv(Domain &dom);

    Res<> close();
};

} // namespace Hjert::Core
