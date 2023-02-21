#pragma once

#include <hal/io.h>
#include <hal/vmm.h>
#include <hjert-api/api.h>
#include <hjert-core/mem.h>
#include <karm-base/lock.h>
#include <karm-base/range-alloc.h>
#include <karm-base/rc.h>
#include <karm-base/vec.h>

namespace Hjert::Core {

struct VNode {
    using _Mem = Var<Hal::PmmMem, Hal::DmaRange>;
    _Mem _mem;

    VNode(_Mem mem);

    static Res<Strong<VNode>> alloc(size_t size, Hj::MemFlags);

    static Res<Strong<VNode>> makeDma(Hal::DmaRange prange);

    Hal::PmmRange range();
};

struct Space {
    struct Map {
        Hal::VmmRange vrange;
        size_t off;
        Strong<VNode> mem;
    };

    Lock _lock;
    RangeAlloc<Hal::VmmRange> _alloc;
    Vec<Map> _maps;

    Space() {
        _alloc.unused(Hal::VmmRange{0x400000, 0x800000000000});
    }

    virtual ~Space() = default;

    virtual Hal::Vmm &vmm() = 0;

    static Res<Strong<Space>> create();

    Res<size_t> _lookup(Hal::VmmRange vrange);

    Res<Hal::VmmRange> map(Hal::VmmRange vrange, Strong<VNode> mem, size_t off, Hj::MapFlags flags);

    Res<> unmap(Hal::VmmRange vrange);
};

} // namespace Hjert::Core
