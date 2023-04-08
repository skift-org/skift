#include <karm-base/checked.h>
#include <karm-logger/logger.h>

#include "arch.h"
#include "space.h"

namespace Hjert::Core {

/* --- VNone ---------------------------------------------------------------- */

Res<Strong<VNode>> VNode::alloc(usize size, Hj::MemFlags) {
    if (size == 0) {
        return Error::invalidInput("size is zero");
    }

    try$(ensureAlign(size, Hal::PAGE_SIZE));
    Hal::PmmMem mem = try$(pmm().allocOwned(size, Hal::PmmFlags::UPPER));
    return Ok(makeStrong<VNode>(std::move(mem)));
}

Res<Strong<VNode>> VNode::makeDma(Hal::DmaRange prange) {
    if (prange.size == 0) {
        return Error::invalidInput("size is zero");
    }

    try$(prange.ensureAligned(Hal::PAGE_SIZE));
    return Ok(makeStrong<VNode>(prange));
}

Hal::PmmRange VNode::range() {
    return _mem.visit(
        Visitor{
            [](Hal::PmmMem const &mem) {
                return mem.range();
            },
            [](Hal::DmaRange const &range) {
                return range.as<Hal::PmmRange>();
            },
        });
}

/* --- Space ---------------------------------------------------------------- */

Res<Strong<Space>> Space::create() {
    return Ok(makeStrong<Space>(try$(Arch::createVmm())));
}

Res<usize> Space::_lookup(Hal::VmmRange vrange) {
    for (usize i = 0; i < _maps.len(); i++) {
        auto &map = _maps[i];
        if (Op::eq(map.vrange, vrange)) {
            return Ok(i);
        }
    }

    return Error::invalidInput("no such mapping");
}

Res<Hal::VmmRange> Space::map(Hal::VmmRange vrange, Strong<VNode> mem, usize off, Hj::MapFlags flags) {
    ObjectLockScope scope(*this);

    try$(vrange.ensureAligned(Hal::PAGE_SIZE));

    if (vrange.size == 0) {
        vrange.size = mem->range().size;
    }

    auto end = try$(checkedAdd(off, vrange.size));

    if (end > mem->range().size) {
        return Error::invalidInput("mapping too large");
    }

    if (vrange.start == 0) {
        vrange = try$(_alloc.alloc(vrange.size));
    } else {
        _alloc.used(vrange);
    }

    auto map = Map{vrange, off, std::move(mem)};

    try$(vmm().mapRange(map.vrange, {map.mem->range().start + map.off, vrange.size}, flags | Hal::VmmFlags::USER));
    _maps.pushBack(std::move(map));

    return Ok(vrange);
}

Res<> Space::unmap(Hal::VmmRange vrange) {
    logInfo("unmap: {x} - {x}", vrange.start, vrange.end());

    ObjectLockScope scope(*this);

    try$(vrange.ensureAligned(Hal::PAGE_SIZE));

    auto id = try$(_lookup(vrange));
    auto &map = _maps[id];
    try$(vmm().free(map.vrange));
    _alloc.unused(map.vrange);
    _maps.removeAt(id);
    return Ok();
}

void Space::activate() {
    vmm().activate();
}

} // namespace Hjert::Core
