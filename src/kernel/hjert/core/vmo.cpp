module;

#include <hal/io.h>
#include <hal/pmm.h>

export module Hjert.Core:vmo;

import :mem;
import :object;

namespace Hjert::Core {

export struct Vmo : BaseObject<Vmo, Hj::Type::VMO> {
    using _Mem = Union<Hal::PmmMem, Hal::DmaRange>;
    _Mem _mem;

    static Res<Arc<Vmo>> alloc(usize size, Flags<Hj::VmoFlags> flags) {
        if (size == 0)
            return Error::invalidInput("size is zero");

        if (not isAlign(size, Hal::PAGE_SIZE))
            return Error::invalidInput("size should be page aligned");

        Hal::PmmMem mem = try$(pmm().allocOwned(size, flags | Hal::PmmFlags::UPPER));
        return Ok(makeArc<Vmo>(std::move(mem)));
    }

    static Res<Arc<Vmo>> makeDma(Hal::DmaRange prange) {
        if (prange.size == 0) {
            return Error::invalidInput("size is zero");
        }

        try$(prange.ensureAligned(Hal::PAGE_SIZE)
                 .okOr(Error::invalidInput("range is not page aligned")));
        return Ok(makeArc<Vmo>(prange));
    }

    Vmo(_Mem mem) : _mem(std::move(mem)) {}

    Hal::PmmRange range() {
        return _mem.visit(
            Visitor{
                [](Hal::PmmMem const& mem) {
                    return mem.range();
                },
                [](Hal::DmaRange const& range) {
                    return range.into<Hal::PmmRange>();
                },
            }
        );
    }
};

} // namespace Hjert::Core
