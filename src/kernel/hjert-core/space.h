#pragma once

#include <karm-base/range-alloc.h>

#include "object.h"
#include "vmo.h"

namespace Hjert::Core {

struct Space : public BaseObject<Space, Hj::Type::SPACE> {
    struct Map {
        Hal::VmmRange vrange;
        usize off;
        Strong<Vmo> vmo;

        Hal::PmmRange prange() {
            return vmo->range().slice(off, vrange.size);
        }
    };

    Strong<Hal::Vmm> _vmm;
    RangeAlloc<Hal::VmmRange> _alloc;
    Vec<Map> _maps;

    static Res<Strong<Space>> create();

    Space(Strong<Hal::Vmm> vmm);

    ~Space() override;

    Res<usize> _lookup(Hal::VmmRange vrange);

    Res<> _ensureNotMapped(Hal::VmmRange vrange);

    Res<> _validate(Hal::VmmRange vrange);

    Res<Hal::VmmRange> map(Hal::VmmRange vrange, Strong<Vmo> vmo, usize off, Hj::MapFlags flags);

    Res<> unmap(Hal::VmmRange vrange);

    void activate();

    void dump();
};

} // namespace Hjert::Core
