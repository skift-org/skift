#pragma once

#include <hal/io.h>

#include "object.h"

namespace Hjert::Core {

struct IoNode : public BaseObject<IoNode> {
    Hal::PortRange _range;

    static Res<Strong<IoNode>> create(Hal::PortRange range);

    IoNode(Hal::PortRange range)
        : BaseObject(Hj::Type::IO), _range(range) {}

    Res<Hj::Arg> in(usize offset, usize size);

    Res<> out(usize offset, usize size, Hj::Arg arg);
};

} // namespace Hjert::Core
