#pragma once

#include <karm-base/ranges.h>

#include "object.h"
#include "vmo.h"

namespace Hjert::Core {

struct Space : public BaseObject<Space, Hj::Type::SPACE> {
    struct Map {
        Hal::VmmRange vrange;
        usize off;
        Arc<Vmo> vmo;

        Hal::PmmRange prange() {
            return vmo->range().slice(off, vrange.size);
        }
    };

    Arc<Hal::Vmm> _vmm;
    Ranges<Hal::VmmRange> _ranges;
    Vec<Map> _maps;

    static Res<Arc<Space>> create();

    Space(Arc<Hal::Vmm> vmm);

    ~Space() override;

    Res<usize> _lookup(Hal::VmmRange vrange);

    Res<> _ensureNotMapped(Hal::VmmRange vrange);

    Res<> _validate(Hal::VmmRange vrange);

    Res<Hal::VmmRange> map(Hal::VmmRange vrange, Arc<Vmo> vmo, usize off, Hj::MapFlags flags);

    Res<> unmap(Hal::VmmRange vrange);

    void activate();

    void dump();
};

} // namespace Hjert::Core
