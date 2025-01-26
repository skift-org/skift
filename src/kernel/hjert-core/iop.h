#pragma once

#include <hal/io.h>

#include "object.h"

namespace Hjert::Core {

struct Iop :
    public BaseObject<Iop, Hj::Type::IOP> {

    Hal::PortRange _range;

    static Res<Arc<Iop>> create(Hal::PortRange range);

    Iop(Hal::PortRange range);

    Res<Hj::Arg> in(usize offset, usize size);

    Res<> out(usize offset, usize size, Hj::Arg arg);
};

} // namespace Hjert::Core
