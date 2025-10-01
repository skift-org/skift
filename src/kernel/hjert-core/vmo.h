#pragma once

#include <hal/io.h>

#include "object.h"

namespace Hjert::Core {

struct Vmo : BaseObject<Vmo, Hj::Type::VMO> {
    using _Mem = Union<Hal::PmmMem, Hal::DmaRange>;
    _Mem _mem;

    static Res<Arc<Vmo>> alloc(usize size, Flags<Hj::VmoFlags>);

    static Res<Arc<Vmo>> makeDma(Hal::DmaRange prange);

    Vmo(_Mem mem) : _mem(std::move(mem)) {}

    Hal::PmmRange range();
};

} // namespace Hjert::Core
