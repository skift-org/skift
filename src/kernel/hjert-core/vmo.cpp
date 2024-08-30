#include "vmo.h"

#include "mem.h"

namespace Hjert::Core {

Res<Strong<Vmo>> Vmo::alloc(usize size, Hj::VmoFlags flags) {
    if (size == 0) {
        return Error::invalidInput("size is zero");
    }

    try$(ensureAlign(size, Hal::PAGE_SIZE));
    Hal::PmmMem mem = try$(pmm().allocOwned(size, flags | Hal::PmmFlags::UPPER));
    return Ok(makeStrong<Vmo>(std::move(mem)));
}

Res<Strong<Vmo>> Vmo::makeDma(Hal::DmaRange prange) {
    if (prange.size == 0) {
        return Error::invalidInput("size is zero");
    }

    try$(prange.ensureAligned(Hal::PAGE_SIZE));
    return Ok(makeStrong<Vmo>(prange));
}

Hal::PmmRange Vmo::range() {
    return _mem.visit(
        Visitor{
            [](Hal::PmmMem const &mem) {
                return mem.range();
            },
            [](Hal::DmaRange const &range) {
                return range.into<Hal::PmmRange>();
            },
        }
    );
}

} // namespace Hjert::Core
