module;

#include <hal/raw.h>

export module Hjert.Core:iop;

import :object;

namespace Hjert::Core {

struct Iop : BaseObject<Iop, Hj::Type::IOP> {

    Hal::PortRange _range;

    static Res<Arc<Iop>> create(Hal::PortRange range) {
        return Ok(makeArc<Iop>(range));
    }

    Iop(Hal::PortRange range) : _range(range) {}

    Res<Hj::Arg> in(usize offset, usize size) {
        ObjectLockScope scope(*this);
        return Hal::RawPortIo(_range)
            .in(offset, size);
    }

    Res<> out(usize offset, usize size, Hj::Arg arg) {
        ObjectLockScope scope(*this);
        auto r = Hal::RawPortIo(_range)
                     .out(offset, size, arg);
        return r;
    }
};

} // namespace Hjert::Core
