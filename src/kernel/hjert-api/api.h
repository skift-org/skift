#pragma once

#include <karm-base/string.h>

#include "syscalls.h"

namespace Hj {

inline Res<usize> log(Str msg) {
    try$(_log(msg.buf(), msg.len()));
    return Ok(msg.len());
}

inline Res<usize> log(Bytes bytes) {
    try$(_log((char const *)bytes.buf(), bytes.len()));
    return Ok(bytes.len());
}

struct Object {
    Cap _cap;

    Object(Cap cap)
        : _cap(cap) {}

    Object(Object &&other)
        : _cap(std::exchange(other._cap, {})) {
    }

    Object(Object const &) = delete;

    ~Object() {
        if (_cap)
            _drop(_cap).unwrap();
    }

    Res<> drop() {
        try$(_drop(_cap));
        _cap = {};
        return Ok();
    }

    Res<> label(Str l) {
        return _label(_cap, l.buf(), l.len());
    }

    Res<> signal(Signals set, Signals unset, Arg value) {
        return _signal(_cap, set, unset, value);
    }

    Res<Arg> wait(Signals set, Signals unset) {
        Cond cond{_cap, set, unset};
        Event event{};
        usize evLen = 1;
        try$(_wait(&cond, 1, &event, &evLen, TimeStamp::endOfTime()));
        return Ok(event.val);
    }

    operator Cap() const {
        return _cap;
    }
};

struct Domain : public Object {
    static Domain self() {
        return Domain{ROOT};
    }

    static Res<Domain> create(Cap dest) {
        Cap cap;
        try$(_createDomain(dest, &cap));
        return Ok(Domain{cap});
    }
};

struct Task : public Object {
    using Object::wait;

    static Task self() {
        return Task{ROOT};
    }

    static Res<Task> create(Cap dest, Cap node, Cap space) {
        Cap cap;
        try$(_createTask(dest, &cap, node, space));
        return Ok(Task{cap});
    }

    Res<> start(usize ip, usize sp, Args args) {
        return _start(_cap, ip, sp, &args);
    }

    Res<Arg> wait() {
        return wait(Signals::EXITED, Signals::NONE);
    }

    Res<> ret(Arg ret = 0) {
        return signal(Signals::EXITED, Signals::NONE, ret);
    }
};

struct Vmo : public Object {
    static Res<Vmo> create(Cap dest, usize phys, usize len, VmoFlags flags = VmoFlags::NONE) {
        Cap cap;
        try$(_createVmo(dest, &cap, phys, len, flags));
        return Ok(Vmo{cap});
    }
};

struct Space : public Object {
    static Space self() {
        return Space{ROOT};
    }

    static Res<Space> create(Cap dest) {
        Cap cap;
        try$(_createSpace(dest, &cap));
        return Ok(Space{cap});
    }

    Res<usize> map(usize virt, Cap vmo, usize off, usize len, MapFlags flags = MapFlags::NONE) {
        try$(_map(_cap, &virt, vmo, off, len, flags));
        return Ok(virt);
    }

    Res<usize> map(Vmo &vmo, usize off, usize len, MapFlags flags = MapFlags::NONE) {
        usize virt = 0;
        try$(_map(_cap, &virt, vmo, off, len, flags));
        return Ok(virt);
    }

    Res<usize> map(Vmo &vmo, MapFlags flags = MapFlags::NONE) {
        usize virt = 0;
        try$(_map(_cap, &virt, vmo, 0, 0, flags));
        return Ok(virt);
    }

    Res<> unmap(usize virt, usize len) {
        return _unmap(_cap, virt, len);
    }
};

struct Io : public Object {
    static Res<Io> create(Cap dest, usize base, usize len) {
        Cap cap;
        try$(_createIo(dest, &cap, base, len));
        return Ok(Io{cap});
    }

    Res<Arg> in(IoLen len, usize port) {
        Arg val;
        try$(_in(_cap, len, port, &val));
        return Ok(val);
    }

    Res<> out(IoLen len, usize port, Arg val) {
        return _out(_cap, len, port, val);
    }
};

} // namespace Hj
