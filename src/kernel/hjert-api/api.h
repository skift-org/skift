#pragma once

#include <karm-base/string.h>

#include "raw.h"

namespace Hj {

inline Res<usize> log(Str msg) {
    try$(_log(msg.buf(), msg.len()));
    return Ok(msg.len());
}

struct RaiiCap {
    Cap _cap;

    RaiiCap(Cap cap)
        : _cap(cap) {}

    RaiiCap(RaiiCap &&other)
        : _cap(std::exchange(other._cap, {})) {
    }

    RaiiCap(RaiiCap const &) = delete;

    ~RaiiCap() {
        if (not _cap.isSpecial())
            _drop(_cap).unwrap();
    }

    Res<> drop() {
        try$(_drop(_cap));
        _cap = {};
        return Ok();
    }

    operator Cap() const {
        return _cap;
    }
};

struct Domain {
    RaiiCap _cap;

    static Domain self() {
        return Domain{CSELF};
    }

    Res<> drop() {
        return _drop(_cap);
    }

    operator Cap() const {
        return _cap;
    }
};

inline Res<Domain> createDomain(Cap dest, usize len) {
    Cap cap;
    try$(_createDomain(dest, &cap, len));
    return Ok(Domain{cap});
}

struct Task {
    RaiiCap _cap;

    static Task self() {
        return Task{CSELF};
    }

    Res<> drop() {
        return _cap.drop();
    }

    Res<> start(usize ip, usize sp, Args args) {
        return _start(_cap, ip, sp, &args);
    }

    Res<Arg> wait() {
        Arg arg;
        try$(_wait(_cap, &arg));
        return Ok(arg);
    }

    Res<> ret(Arg ret = 0) {
        return _ret(_cap, ret);
    }

    operator Cap() const {
        return _cap;
    }
};

inline Res<Task> createTask(Cap dest, Cap node, Cap space) {
    Cap cap;
    try$(_createTask(dest, &cap, node, space));
    return Ok(Task{cap});
}

struct Space {
    RaiiCap _cap;

    static Space self() {
        return Space{CSELF};
    }

    Res<> drop() {
        return _cap.drop();
    }

    Res<> map(usize *virt, Cap mem, usize off, usize len, MapFlags flags = MapFlags::NONE) {
        return _map(_cap, virt, mem, off, len, flags);
    }

    Res<usize> map(Cap mem, usize off, usize len, MapFlags flags = MapFlags::NONE) {
        usize virt = 0;
        try$(_map(_cap, &virt, mem, off, len, flags));
        return Ok(virt);
    }

    Res<> unmap(usize virt, usize len) {
        return _unmap(_cap, virt, len);
    }

    operator Cap() const {
        return _cap;
    }
};

inline Res<Space> createSpace(Cap dest) {
    Cap cap;
    try$(_createSpace(dest, &cap));
    return Ok(Space{cap});
}

struct Mem {
    RaiiCap _cap;

    Res<> drop() {
        return _cap.drop();
    }

    operator Cap() const {
        return _cap;
    }
};

inline Res<Mem> createMem(Cap dest, usize phys, usize len, MemFlags flags = MemFlags::NONE) {
    Cap cap;
    try$(_createMem(dest, &cap, phys, len, flags));
    return Ok(Mem{cap});
}

struct Io {
    RaiiCap _cap;

    Res<> drop() {
        return _cap.drop();
    }

    Res<Arg> in(IoLen len, usize port) {
        Arg val;
        try$(_in(_cap, len, port, &val));
        return Ok(val);
    }

    Res<> out(IoLen len, usize port, Arg val) {
        return _out(_cap, len, port, val);
    }

    operator Cap() const {
        return _cap;
    }
};

inline Res<Io> createIo(Cap dest, usize base, usize len) {
    Cap cap;
    try$(_createIo(dest, &cap, base, len));
    return Ok(Io{cap});
}

} // namespace Hj
