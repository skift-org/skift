#pragma once

#include <hal/io.h>
#include <hal/vmm.h>
#include <hjert-api/raw.h>
#include <hjert-core/mem.h>
#include <karm-base/lock.h>
#include <karm-base/range-alloc.h>
#include <karm-base/rc.h>
#include <karm-base/vec.h>
#include <karm-logger/logger.h>

#include "object.h"

namespace Hjert::Core {

struct VNode : public BaseObject<VNode> {
    using _Mem = Var<Hal::PmmMem, Hal::DmaRange>;
    _Mem _mem;

    VNode(_Mem mem)
        : BaseObject(Hj::Type::MEM),
          _mem(std::move(mem)) {}

    static Res<Strong<VNode>> alloc(usize size, Hj::MemFlags);

    static Res<Strong<VNode>> makeDma(Hal::DmaRange prange);

    Hal::PmmRange range();

    bool isDma() {
        return _mem.is<Hal::DmaRange>();
    }
};

struct Space : public BaseObject<Space> {
    struct Map {
        Hal::VmmRange vrange;
        usize off;
        Strong<VNode> mem;
    };

    Strong<Hal::Vmm> _vmm;
    RangeAlloc<Hal::VmmRange> _alloc;
    Vec<Map> _maps;

    Space(Strong<Hal::Vmm> vmm)
        : BaseObject(Hj::Type::SPACE),
          _vmm(vmm) {
        _alloc.unused(Hal::VmmRange{0x400000, 0x800000000000});
    }

    ~Space() {
        while (_maps.len()) {
            unmap(_maps.peekFront().vrange)
                .unwrap("unmap failed");
        }
    }

    static Res<Strong<Space>> create();

    Res<usize> _lookup(Hal::VmmRange vrange);

    Res<Hal::VmmRange> map(Hal::VmmRange vrange, Strong<VNode> mem, usize off, Hj::MapFlags flags);

    Res<> unmap(Hal::VmmRange vrange);

    void activate();
};

} // namespace Hjert::Core
