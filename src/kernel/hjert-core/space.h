#pragma once

#include <hal/io.h>
#include <hal/vmm.h>
#include <hjert-api/api.h>
#include <hjert-core/mem.h>
#include <karm-base/lock.h>
#include <karm-base/range-alloc.h>
#include <karm-base/rc.h>
#include <karm-base/vec.h>

#include "object.h"

namespace Hjert::Core {

struct VNode : public BaseObject<VNode> {
    using _Mem = Var<Hal::PmmMem, Hal::DmaRange>;
    _Mem _mem;

    VNode(_Mem mem);

    static Res<Strong<VNode>> alloc(usize size, Hj::MemFlags);

    static Res<Strong<VNode>> makeDma(Hal::DmaRange prange);

    Hal::PmmRange range();
};

struct Space : public BaseObject<Space> {
    struct Map {
        Hal::VmmRange vrange;
        usize off;
        Strong<VNode> mem;
    };

    RangeAlloc<Hal::VmmRange> _alloc;
    Vec<Map> _maps;

    Space() {
        _alloc.unused(Hal::VmmRange{0x400000, 0x800000000000});
    }

    virtual ~Space() = default;

    virtual Hal::Vmm &vmm() = 0;

    static Res<Strong<Space>> create();

    Res<usize> _lookup(Hal::VmmRange vrange);

    Res<Hal::VmmRange> map(Hal::VmmRange vrange, Strong<VNode> mem, usize off, Hj::MapFlags flags);

    Res<> unmap(Hal::VmmRange vrange);

    void activate();
};

} // namespace Hjert::Core
