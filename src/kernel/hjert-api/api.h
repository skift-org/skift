#pragma once

#include <karm-base/string.h>

#include "raw.h"

namespace Hj {

inline Res<usize> log(Str msg) {
    try$(_log(msg.buf(), msg.len()));
    return Ok(msg.len());
}

inline Res<usize> log(Bytes bytes) {
    try$(_log((char const *)bytes.buf(), bytes.len()));
    return Ok(bytes.len());
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
        if (_cap)
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
        return Domain{ROOT};
    }

    Res<> drop() {
        return _drop(_cap);
    }

    Res<> label(Str l) {
        return _label(_cap, l.buf(), l.len());
    }

    operator Cap() const {
        return _cap;
    }
};

inline Res<Domain> createDomain(Cap dest) {
    Cap cap;
    try$(_createDomain(dest, &cap));
    return Ok(Domain{cap});
}

struct Task {
    RaiiCap _cap;

    static Task self() {
        return Task{ROOT};
    }

    Res<> drop() {
        return _cap.drop();
    }

    Res<> label(Str l) {
        return _label(_cap, l.buf(), l.len());
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

struct Mapping {
};

struct Space {
    RaiiCap _cap;

    static Space self() {
        return Space{ROOT};
    }

    Res<> drop() {
        return _cap.drop();
    }

    Res<> label(Str l) {
        return _label(_cap, l.buf(), l.len());
    }

    Res<usize> map(usize virt, Cap vmo, usize off, usize len, MapFlags flags = MapFlags::NONE) {
        try$(_map(_cap, &virt, vmo, off, len, flags));
        return Ok(virt);
    }

    Res<usize> map(Cap vmo, usize off, usize len, MapFlags flags = MapFlags::NONE) {
        usize virt = 0;
        try$(_map(_cap, &virt, vmo, off, len, flags));
        return Ok(virt);
    }

    Res<usize> map(Cap vmo, MapFlags flags = MapFlags::NONE) {
        usize virt = 0;
        try$(_map(_cap, &virt, vmo, 0, 0, flags));
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

struct Vmo {
    RaiiCap _cap;

    Res<> drop() {
        return _cap.drop();
    }

    Res<> label(Str l) {
        return _label(_cap, l.buf(), l.len());
    }

    operator Cap() const {
        return _cap;
    }
};

inline Res<Vmo> createVmo(Cap dest, usize phys, usize len, VmoFlags flags = VmoFlags::NONE) {
    Cap cap;
    try$(_createVmo(dest, &cap, phys, len, flags));
    return Ok(Vmo{cap});
}

struct Io {
    RaiiCap _cap;

    Res<> drop() {
        return _cap.drop();
    }

    Res<> label(Str l) {
        return _label(_cap, l.buf(), l.len());
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
