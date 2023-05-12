#pragma once

#include <hal/io.h>
#include <hal/vmm.h>
#include <hjert-api/types.h>
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

    static Res<Strong<VNode>> alloc(usize size, Hj::VmoFlags);

    static Res<Strong<VNode>> makeDma(Hal::DmaRange prange);

    VNode(_Mem mem)
        : BaseObject(Hj::Type::VMO),
          _mem(std::move(mem)) {}

    Hal::PmmRange range();

    bool isDma() {
        return _mem.is<Hal::DmaRange>();
    }
};

struct Space : public BaseObject<Space> {
    struct Map {
        Hal::VmmRange vrange;
        usize off;
        Strong<VNode> vmo;
    };

    Strong<Hal::Vmm> _vmm;
    RangeAlloc<Hal::VmmRange> _alloc;
    Vec<Map> _maps;

    Space(Strong<Hal::Vmm> vmm)
        : BaseObject(Hj::Type::SPACE),
          _vmm(vmm) {
        _alloc.unused({0x400000, 0x800000000000});
    }

    ~Space() {
        while (_maps.len()) {
            unmap(_maps.peekFront().vrange)
                .unwrap("unmap failed");
        }
    }

    static Res<Strong<Space>> create();

    Res<usize> _lookup(Hal::VmmRange vrange);

    Res<> _validate(Hal::VmmRange vrange) {
        for (auto &map : _maps) {
            if (map.vrange.contains(vrange)) {
                return Ok();
            }
        }

        return Error::invalidInput("bad address");
    }

    Res<Hal::VmmRange> map(Hal::VmmRange vrange, Strong<VNode> vmo, usize off, Hj::MapFlags flags);

    Res<> unmap(Hal::VmmRange vrange);

    void activate();
};

template <typename T>
struct User {
    usize _addr;

    User(Hj::Arg addr)
        : _addr(addr) {
    }

    Hal::VmmRange vrange() {
        return Hal::VmmRange{_addr, sizeof(T)};
    }

    Res<T> load(Space &space) {
        if (_addr == 0) {
            return Error::invalidInput("null pointer");
        }

        ObjectLockScope scope(space);
        try$(space._validate(vrange()));
        return Ok(*reinterpret_cast<T *>(_addr));
    }

    Res<> store(Space &space, T const &val) {
        if (_addr == 0) {
            return Error::invalidInput("null pointer");
        }

        ObjectLockScope scope(space);
        try$(space._validate(vrange()));
        *reinterpret_cast<T *>(_addr) = val;
        return Ok();
    }
};

template <typename T>
struct UserSlice {
    usize _addr;
    usize _len;

    UserSlice(Hj::Arg addr, usize len)
        : _addr(addr),
          _len(len) {
    }

    Hal::VmmRange vrange() const {
        return Hal::VmmRange{_addr, sizeof(T) * _len};
    }

    usize len() const {
        return _len;
    }

    template <typename R>
    Res<> with(Space &space, auto f) {
        if (_addr == 0) {
            return Error::invalidInput("null pointer");
        }

        ObjectLockScope scope(space);
        try$(space._validate(vrange()));
        return f(R{reinterpret_cast<T *>(_addr), _len});
    }

    Res<> store(T const &val);

    Res<T> load() const;
};

} // namespace Hjert::Core
