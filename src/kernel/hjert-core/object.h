#pragma once

#include <hjert-api/types.h>
#include <karm-base/atomic.h>
#include <karm-base/lock.h>
#include <karm-base/rc.h>
#include <karm-io/fmt.h>

namespace Hjert::Core {

struct Object : Meta::Pinned {
    static Atomic<usize> _counter;

    Lock _lock;
    usize _id = _counter.fetchAdd(1);
    Opt<String> _label;
    Flags<Hj::Sigs> _signals;

    virtual ~Object() = default;

    virtual Hj::Type type() const = 0;

    usize id() const { return _id; }

    void label(Str label);

    String label() const;

    void _signalUnlock(Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset);

    Flags<Hj::Sigs> _pollUnlock();

    void signal(Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset);

    Flags<Hj::Sigs> poll();
};

template <typename Crtp, Hj::Type _TYPE>
struct BaseObject : public Object {
    using _Crtp = Crtp;
    static constexpr Hj::Type TYPE = _TYPE;
    using Object::Object;

    Hj::Type type() const override {
        return TYPE;
    }
};

struct [[nodiscard]] ObjectLockScope : public LockScope<Lock> {
    ObjectLockScope(Object &obj)
        : LockScope(obj._lock) {
    }
};

} // namespace Hjert::Core

template <Meta::Derive<Hjert::Core::Object> T>
struct Karm::Io::Formatter<T> {
    Res<usize> format(Io::TextWriter &writer, Hjert::Core::Object const &obj) {
        return Io::format(writer, "{}({}, '{}')", Io::toPascalCase(Hj::toStr(obj.type())).unwrap(), obj.id(), obj.label());
    }
};
