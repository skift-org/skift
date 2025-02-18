#include "vmo.h"

#include "mem.h"

namespace Hjert::Core {

Res<Arc<Vmo>> Vmo::alloc(usize size, Hj::VmoFlags flags) {
    if (size == 0) {
        return Error::invalidInput("size is zero");
    }

    try$(ensureAlign(size, Hal::PAGE_SIZE));
    Hal::PmmMem mem = try$(pmm().allocOwned(size, flags | Hal::PmmFlags::UPPER));
    return Ok(makeArc<Vmo>(std::move(mem)));
}

Res<Arc<Vmo>> Vmo::makeDma(Hal::DmaRange prange) {
    if (prange.size == 0) {
        return Error::invalidInput("size is zero");
    }

    try$(prange.ensureAligned(Hal::PAGE_SIZE));
    return Ok(makeArc<Vmo>(prange));
}

Hal::PmmRange Vmo::range() {
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

} // namespace Hjert::Core
