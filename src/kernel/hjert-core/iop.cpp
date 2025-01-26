#include <hal/raw.h>

#include "iop.h"

namespace Hjert::Core {

Res<Arc<Iop>> Iop::create(Hal::PortRange range) {
    return Ok(makeArc<Iop>(range));
}

Iop::Iop(Hal::PortRange range)
    : _range(range) {}

Res<Hj::Arg> Iop::in(usize offset, usize size) {
    ObjectLockScope scope(*this);
    return Hal::RawPortIo(_range)
        .in(offset, size);
}

Res<> Iop::out(usize offset, usize size, Hj::Arg arg) {
    ObjectLockScope scope(*this);
    auto r = Hal::RawPortIo(_range)
                 .out(offset, size, arg);
    return r;
}

} // namespace Hjert::Core
