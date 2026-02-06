module;

#include <hal/mem.h>
#include <hal/vmm.h>
#include <karm/macros>

export module Hjert.Core:space;

import Karm.Core;
import :object;
import :vmo;

namespace Hjert::Core {

struct Space : BaseObject<Space, Hj::Type::SPACE> {
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

    static Res<Arc<Space>> create() {
        return Ok(makeArc<Space>(try$(Arch::createVmm())));
    }

    Space(Arc<Hal::Vmm> vmm) : _vmm(vmm) {
#ifdef __ck_bits_64__
        _ranges.add({Hal::PAGE_SIZE, 0x800000000000});
#else
        _ranges.add({Hal::PAGE_SIZE, 0xC0000000});
#endif
    }

    ~Space() override {
        while (_maps.len()) {
            unmap(first(_maps).vrange)
                .unwrap("unmap failed");
        }
    }

    Res<usize> _lookup(Hal::VmmRange vrange) {
        for (usize i = 0; i < _maps.len(); i++) {
            auto& map = _maps[i];
            if (map.vrange == vrange) {
                return Ok(i);
            }
        }

        return Error::invalidInput("no such mapping");
    }

    Res<> _ensureNotMapped(Hal::VmmRange vrange) {
        for (usize i = 0; i < _maps.len(); i++) {
            auto& map = _maps[i];
            if (map.vrange.overlaps(vrange)) {
                return Error::invalidInput("already mapped");
            }
        }

        return Ok();
    }

    Res<> _validate(Hal::VmmRange vrange) {
        for (auto& map : _maps) {
            if (map.vrange.contains(vrange)) {
                return Ok();
            }
        }

        return Error::invalidInput("bad address");
    }

    Res<Hal::VmmRange> map(Hal::VmmRange vrange, Arc<Vmo> vmo, usize off, Flags<Hj::MapFlags> flags) {
        ObjectLockScope _{*this};

        try$(vrange.ensureAligned(Hal::PAGE_SIZE));

        if (vrange.size == 0)
            vrange.size = vmo->range().size;

        auto end = try$(checkedAdd(off, vrange.size));

        if (end > vmo->range().size)
            return Error::invalidInput("mapping too large");

        if (vrange.start == 0) {
            vrange = try$(_ranges.take(vrange.size));
        } else {
            try$(_ensureNotMapped(vrange));
            _ranges.remove(vrange);
        }

        Map map = {vrange, off, std::move(vmo)};

        Hal::PmmRange prange = {map.vmo->range().start + map.off, vrange.size};
        try$(_vmm->mapRange(map.vrange, prange, flags | Hal::VmmFlags::USER));
        try$(_vmm->flush(map.vrange));

        _maps.pushBack(std::move(map));

        return Ok(vrange);
    }

    Res<> unmap(Hal::VmmRange vrange) {
        ObjectLockScope _{*this};

        try$(vrange.ensureAligned(Hal::PAGE_SIZE));

        auto id = try$(_lookup(vrange));
        auto& map = _maps[id];

        try$(_vmm->free(map.vrange));
        try$(_vmm->flush(map.vrange));

        _ranges.add(map.vrange);
        _maps.removeAt(id);
        return Ok();
    }

    void activate() {
        _vmm->activate();
    }

    void dump() {
        ObjectLockScope _{*this};

        for (auto& map : _maps) {
            auto vrange = map.vrange;
            auto prange = map.prange();
            auto size = vrange.size / 1024;
            logDebug("{}: map: {x}-{x} -> {x}-{x} {} {}kib", *this, vrange.start, vrange.end(), prange.start, prange.end(), map.vmo->label(), size);
        }

        _vmm->dump();
    }
};

} // namespace Hjert::Core
