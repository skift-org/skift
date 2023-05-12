#pragma once

#include <karm-base/ring.h>

#include "object.h"

namespace Hjert::Core {

struct Parcel {
};

struct Channel :
    public BaseObject<Channel> {

    Ring<Parcel> _ring;

    Res<> send(Hj::Msg msg, Domain &domain);

    Res<> recv(Hj::Msg msg, Domain &domain);
};

} // namespace Hjert::Core
