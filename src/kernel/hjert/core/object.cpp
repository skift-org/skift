export module Hjert.Core:object;

import Karm.Core;
import Hjert.Api;

using namespace Karm;

namespace Hjert::Core {

export struct Object : Meta::Pinned {
    static Atomic<usize> _counter;

    Lock _lock;
    usize _id = _counter.fetchAdd(1);
    Opt<String> _label;
    Flags<Hj::Sigs> _signals;

    virtual ~Object() = default;

    virtual Hj::Type type() const = 0;

    usize id() const { return _id; }

    void label(Str label) {
        LockScope scope(_lock);
        _label = String(label);
    }

    String label() const {
        if (not _label)
            return "<unlabeled>"s;
        return *_label;
    }

    void _signalUnlock(Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset) {
        _signals |= set;
        _signals &= ~unset;
    }

    Flags<Hj::Sigs> _pollUnlock() {
        return _signals;
    }

    void signal(Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset) {
        LockScope scope(_lock);
        _signalUnlock(set, unset);
    }

    Flags<Hj::Sigs> poll() {
        LockScope scope(_lock);
        return _pollUnlock();
    }
};

Atomic<usize> Object::_counter = 0;

export template <typename Crtp, Hj::Type _TYPE>
struct BaseObject : Object {
    using _Crtp = Crtp;
    static constexpr Hj::Type TYPE = _TYPE;
    using Object::Object;

    Hj::Type type() const override {
        return TYPE;
    }
};

export struct [[nodiscard]] ObjectLockScope : LockScope<Lock> {
    ObjectLockScope(Object& obj)
        : LockScope(obj._lock) {
    }
};

} // namespace Hjert::Core

export template <Meta::Derive<Hjert::Core::Object> T>
struct Karm::Io::Formatter<T> {
    Res<> format(Io::TextWriter& writer, Hjert::Core::Object const& obj) {
        return Io::format(writer, "{}({}, '{}')", Io::toPascalCase(Hj::toStr(obj.type())).unwrap(), obj.id(), obj.label());
    }
};
