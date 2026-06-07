module;

#include <karm/macros>

export module Hjert.Api:objects;

import Karm.Core;
import :syscalls;
import :types;

using namespace Karm::Literals;

namespace Hj {

export Res<Instant> now() {
    Instant ts;
    try$(_now(&ts));
    return Ok(ts);
}

export Res<usize> log(Str msg) {
    try$(_log(msg.buf(), msg.len()));
    return Ok(msg.len());
}

export Res<usize> log(Bytes bytes) {
    try$(_log((char const*)bytes.buf(), bytes.len()));
    return Ok(bytes.len());
}

export struct Object {
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

    Res<Hj::Cap> dup(Hj::Cap dest) {
        Hj::Cap out;
        try$(Hj::_dup(dest, &out, _cap));
        return Ok(out);
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

export struct Domain : Object {
    using Object::Object;

    static Domain self() {
        return Domain{ROOT};
    }

    static Res<Domain> create(Cap dest) {
        Cap cap;
        try$(_createDomain(dest, &cap));
        return Ok(Domain{cap});
    }

    Res<Cap> attach(Object& obj) {
        Cap out;
        try$(_dup(_cap, &out, obj));
        return Ok(out);
    }
};

export struct Task : Object {
    using Object::Object;

    static Task self() {
        return Task{ROOT};
    }

    static Res<Task> create(Cap dest, Cap job, Cap domain, Cap space) {
        Cap cap;
        try$(_createTask(dest, &cap, job, domain, space));
        return Ok(Task{cap});
    }

    Res<> start(usize ip, usize sp, Args args) {
        return _start(_cap, ip, sp, &args);
    }

    Res<> ret() {
        return signal(Sigs::EXITED, Sigs::NONE);
    }

    Res<> crash() {
        return signal({Sigs::EXITED, Sigs::CRASHED}, Sigs::NONE);
    }
};

export struct Vmo : Object {
    using Object::Object;

    static Res<Vmo> create(Cap dest, usize phys, usize len, Flags<VmoFlags> flags = {}, Cap parent = ROOT) {
        Cap cap;
        try$(_createVmo(dest, &cap, phys, len, flags, parent));
        return Ok(Vmo{cap});
    }
};

export struct Space : Object {
    using Object::Object;

    static Space self() {
        return Space{ROOT};
    }

    static Res<Space> create(Cap dest) {
        Cap cap;
        try$(_createSpace(dest, &cap));
        return Ok(Space{cap});
    }

    Res<urange> map(usize virt, Vmo& vmo, usize off, usize len, Flags<MapFlags> flags = {}) {
        try$(_map(_cap, &virt, vmo, off, &len, flags));
        return Ok(urange{virt, len});
    }

    Res<urange> map(Vmo& vmo, usize off, usize len, Flags<MapFlags> flags = {}) {
        usize virt = 0;
        try$(_map(_cap, &virt, vmo, off, &len, flags));
        return Ok(urange{virt, len});
    }

    Res<urange> map(Vmo& vmo, Flags<MapFlags> flags = {}) {
        usize virt = 0;
        usize len = 0;
        try$(_map(_cap, &virt, vmo, 0, &len, flags));
        return Ok(urange{virt, len});
    }

    Res<> unmap(urange range) {
        return _unmap(_cap, range.start, range.size);
    }
};

export struct Mapped {
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

export Res<Mapped> map(usize virt, Vmo& vmo, usize off, usize len, Flags<MapFlags> flags = {}) {
    try$(_map(Space::self(), &virt, vmo, off, &len, flags));
    return Ok(Mapped{virt, len});
}

export Res<Mapped> map(Vmo& vmo, usize off, usize len, Flags<MapFlags> flags = {}) {
    usize virt = 0;
    try$(_map(Space::self(), &virt, vmo, off, &len, flags));
    return Ok(Mapped{virt, len});
}

export Res<Mapped> map(Vmo& vmo, Flags<MapFlags> flags = {}) {
    usize virt = 0;
    usize len = 0;
    try$(_map(Space::self(), &virt, vmo, 0, &len, flags));
    return Ok(Mapped{virt, len});
}

export struct Io : Object {
    using Object::Object;

    static Res<Io> create(Cap dest, usize base, usize len) {
        Cap cap;
        try$(_createIop(dest, &cap, base, len));
        return Ok(Io{cap});
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

export struct Channel : Object {
    using Object::Object;

    static Res<Pair<Channel>> create(Cap dest, usize bufLen, usize capLen) {
        Cap cap0;
        Cap cap1;
        try$(_createChannel(dest, &cap0, &cap1, bufLen, capLen));
        return Ok(Pair<Channel>{cap0, cap1});
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

export struct Pipe : Object {
    using Object::Object;

    static Res<Pipe> create(Cap dest, usize bufLen) {
        Cap cap;
        try$(_createPipe(dest, &cap, bufLen));
        return Ok(Pipe{cap});
    }

    Res<usize> write(Bytes buf) {
        usize bufLen = buf.len();
        try$(_write(_cap, buf.buf(), &bufLen));
        return Ok(bufLen);
    }

    Res<usize> read(MutBytes buf) {
        usize bufLen = buf.len();
        try$(_read(_cap, buf.buf(), &bufLen));
        return Ok(bufLen);
    }
};

export struct Irq : Object {
    using Object::Object;

    static Res<Irq> create(Cap dest, usize irq) {
        Cap cap;
        try$(_createIrq(dest, &cap, irq));
        return Ok(Irq{cap});
    }

    Res<> eoi() {
        return _eoi(cap());
    }

    Res<> bind(Cap pipeCap) {
        return _bind(cap(), pipeCap);
    }
};

export struct Listener : Object {
    using Object::Object;

    Buf<Event> _evs = {};
    usize _len = {};

    static Res<Listener> create(Cap dest) {
        Cap cap;
        try$(_createListener(dest, &cap));
        return Ok(Listener{cap});
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

export struct Clock : Object {
    using Object::Object;

    static Res<Clock> create(Cap dest) {
        Cap cap;
        try$(_createClock(dest, &cap));
        return Ok(Clock{cap});
    }
};

export struct Job : Object {
    using Object::Object;

    static Res<Job> create(Cap dest, Cap parent) {
        Cap cap;
        try$(_createJob(dest, &cap, parent));
        return Ok(Job{cap});
    }
};

} // namespace Hj

export template <Meta::Derive<Hj::Object> T>
struct Karm::Serde::Serde<T> {
    static Res<> serialize(Serializer& ser, T const& v) {
        auto scope = try$(ser.beginScope({.kind = Type::UNIT, .tag = "__handle__"_sym}));
        try$(scope.serialize(v._cap.raw()));
        return scope.end();
    }

    static Res<T> deserialize(Deserializer& de) {
        auto scope = try$(de.beginScope({.kind = Type::UNIT, .tag = "__handle__"_sym}));
        Hj::Cap cap{try$(scope.deserialize<Hj::Arg>())};
        try$(scope.end());
        return Ok<T>(cap);
    }
};
