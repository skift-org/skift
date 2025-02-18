#pragma once

#include <karm-base/string.h>
#include <karm-io/pack.h>

#include "syscalls.h"

namespace Hj {

inline Res<Instant> now() {
    Instant ts;
    try$(_now(&ts));
    return Ok(ts);
}

inline Res<usize> log(Str msg) {
    try$(_log(msg.buf(), msg.len()));
    return Ok(msg.len());
}

inline Res<usize> log(Bytes bytes) {
    try$(_log((char const*)bytes.buf(), bytes.len()));
    return Ok(bytes.len());
}

template <typename O, typename... Args>
inline Res<O> create(Cap dest, Args&&... args) {
    Cap c;
    typename O::Props props{std::forward<Args>(args)...};
    Props p = props;
    try$(_create(dest, &c, &p));
    return Ok(O{c});
}

struct Object {
    Cap _cap;

    Object(Cap cap)
        : _cap(cap) {}

    Object(Object&& other)
        : _cap(std::exchange(other._cap, {})) {
    }

    Object(Object const&) = delete;

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

    Res<> signal(Flags<Sigs> set, Flags<Sigs> unset) {
        return _signal(_cap, set, unset);
    }

    operator Cap() const {
        return _cap;
    }

    Arg raw() const {
        return _cap.raw();
    }

    Cap cap() const {
        return _cap;
    }
};

struct Domain : public Object {
    using Props = DomainProps;

    using Object::Object;

    static Domain self() {
        return Domain{ROOT};
    }

    static Res<Domain> create(Cap dest) {
        return Hj::create<Domain>(dest);
    }

    template <typename O, typename... Args>
    Res<O> create(Args&&... args) {
        return Hj::create<O>(_cap, std::forward<Args>(args)...);
    }

    Res<Cap> attach(Object& obj) {
        Cap out;
        try$(_dup(_cap, &out, obj));
        return Ok(out);
    }
};

struct Task : public Object {
    using Props = TaskProps;

    using Object::Object;

    static Task self() {
        return Task{ROOT};
    }

    static Res<Task> create(Cap dest, Cap node, Cap space) {
        return create<Task>(dest, node, space);
    }

    Res<> start(usize ip, usize sp, Args args) {
        return _start(_cap, ip, sp, &args);
    }

    Res<> ret() {
        return signal(Sigs::EXITED, Sigs::NONE);
    }

    Res<> crash() {
        return signal(Sigs::EXITED | Sigs::CRASHED, Sigs::NONE);
    }
};

struct Vmo : public Object {
    using Props = VmoProps;

    using Object::Object;

    static Res<Vmo> create(Cap dest, usize phys, usize len, VmoFlags flags = VmoFlags::NONE) {
        return create<Vmo>(dest, phys, len, flags);
    }
};

struct Space : public Object {
    using Props = SpaceProps;

    using Object::Object;

    static Space self() {
        return Space{ROOT};
    }

    static Res<Space> create(Cap dest) {
        return create<Space>(dest);
    }

    Res<urange> map(usize virt, Vmo& vmo, usize off, usize len, MapFlags flags = MapFlags::NONE) {
        try$(_map(_cap, &virt, vmo, off, &len, flags));
        return Ok(urange{virt, len});
    }

    Res<urange> map(Vmo& vmo, usize off, usize len, MapFlags flags = MapFlags::NONE) {
        usize virt = 0;
        try$(_map(_cap, &virt, vmo, off, &len, flags));
        return Ok(urange{virt, len});
    }

    Res<urange> map(Vmo& vmo, MapFlags flags = MapFlags::NONE) {
        usize virt = 0;
        usize len = 0;
        try$(_map(_cap, &virt, vmo, 0, &len, flags));
        return Ok(urange{virt, len});
    }

    Res<> unmap(urange range) {
        return _unmap(_cap, range.start, range.size);
    }
};

struct Mapped {
    usize _addr{};
    usize _len{};

    Mapped(usize addr, usize len)
        : _addr(addr), _len(len) {}

    Mapped(Mapped const&) = delete;

    Mapped(Mapped&& other)
        : _addr(std::exchange(other._addr, {})),
          _len(std::exchange(other._len, {})) {
    }

    ~Mapped() {
        if (_addr)
            Space::self().unmap(range()).unwrap();
    }

    urange range() const { return {_addr, _len}; }

    MutBytes mutBytes() { return {reinterpret_cast<u8*>(_addr), _len}; }

    Bytes bytes() const { return {reinterpret_cast<u8 const*>(_addr), _len}; }
};

static inline Res<Mapped> map(usize virt, Vmo& vmo, usize off, usize len, MapFlags flags = MapFlags::NONE) {
    try$(_map(Space::self(), &virt, vmo, off, &len, flags));
    return Ok(Mapped{virt, len});
}

static inline Res<Mapped> map(Vmo& vmo, usize off, usize len, MapFlags flags = MapFlags::NONE) {
    usize virt = 0;
    try$(_map(Space::self(), &virt, vmo, off, &len, flags));
    return Ok(Mapped{virt, len});
}

static inline Res<Mapped> map(Vmo& vmo, MapFlags flags = MapFlags::NONE) {
    usize virt = 0;
    usize len = 0;
    try$(_map(Space::self(), &virt, vmo, 0, &len, flags));
    return Ok(Mapped{virt, len});
}

struct Io : public Object {
    using Props = IopProps;

    using Object::Object;

    static Res<Io> create(Cap dest, usize base, usize len) {
        return create<Io>(dest, base, len);
    }

    Res<Arg> in(IoLen len, usize port) {
        Arg val = 0;
        try$(_in(_cap, len, port, &val));
        return Ok(val);
    }

    Res<> out(IoLen len, usize port, Arg val) {
        return _out(_cap, len, port, val);
    }
};

struct Channel : public Object {
    using Props = ChannelProps;

    using Object::Object;

    static Res<Channel> create(Cap dest, usize bufLen, usize capLen) {
        return create<Channel>(dest, bufLen, capLen);
    }

    Res<SentRecv> send(Bytes buf, Slice<Cap> caps) {
        try$(_send(_cap, buf.buf(), buf.len(), caps.buf(), caps.len()));
        return Ok<SentRecv>(buf.len(), caps.len());
    }

    Res<SentRecv> recv(MutBytes buf, MutSlice<Cap> caps) {
        usize bufLen = buf.len();
        usize capLen = caps.len();
        try$(_recv(_cap, buf.buf(), &bufLen, caps.buf(), &capLen));
        return Ok<SentRecv>(bufLen, capLen);
    }
};

struct Irq : public Object {
    using Props = IrqProps;

    using Object::Object;

    static Res<Irq> create(Cap dest, usize irq) {
        return create<Irq>(dest, irq);
    }
};

struct Listener : public Object {
    using Props = ListenerProps;

    using Object::Object;

    Buf<Event> _evs = {};
    usize _len = {};

    static Res<Listener> create(Cap dest) {
        return create<Listener>(dest);
    }

    Res<> listen(Cap cap, Flags<Sigs> set, Flags<Sigs> unset) {
        return _listen(_cap, cap, set, unset);
    }

    Res<> mute(Cap cap) {
        return _listen(_cap, cap, Sigs::NONE, Sigs::NONE);
    }

    Res<> poll(Instant until) {
        _evs.resize(256);
        _len = 0;
        return _poll(_cap, _evs.buf(), _evs.len(), &_len, until);
    }

    Opt<Event> next() {
        if (_len) {
            auto ev = _evs[_len - 1];
            _len--;
            return ev;
        }

        return NONE;
    }
};

} // namespace Hj

template <Meta::Derive<Hj::Object> T>
struct Karm::Io::Packer<T> {
    static Res<> pack(PackEmit& e, T const& val) {
        e.give(Sys::Handle{val.raw()});
        return Ok();
    }

    static Res<T> unpack(PackScan& s) {
        auto cap = s.take();
        return Ok(T{Hj::Cap{cap.value()}});
    }
};
