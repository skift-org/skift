#pragma once

#include <hjert-api/types.h>
#include <karm-base/array.h>
#include <karm-base/lock.h>
#include <karm-base/rc.h>
#include <karm-base/res.h>
#include <karm-fmt/case.h>
#include <karm-logger/logger.h>

namespace Hjert::Core {

/* --- Object --------------------------------------------------------------- */

struct Object : Meta::Static {
    static Atomic<usize> _counter;

    Lock _lock;

    usize _id;
    Hj::Type _type;
    Opt<String> _label;

    Hj::Signals _signals;
    Array<u64, 32> _data;

    Object(Hj::Type type)
        : _id(_counter.fetchAdd(1)),
          _type(type) {
    }

    usize id() const {
        return _id;
    }

    Hj::Type type() {
        return _type;
    }

    void label(Str label) {
        LockScope scope(_lock);
        _label = String(label);
    }

    Str label() const {
        if (_label) {
            return _label.unwrap();
        }
        return "<no label>";
    }

    virtual ~Object() {
        logInfo("object: destroyed {}", *this);
    };
};

template <typename Crtp>
struct BaseObject : public Object {
    using _Crtp = Crtp;
    using Object::Object;
};

/* --- Domain --------------------------------------------------------------- */

using Slot = Opt<Strong<Object>>;

struct Domain : public BaseObject<Domain> {
    static constexpr usize SHIFT = 11;
    static constexpr usize LEN = 2 << 11;
    static constexpr usize MASK = LEN - 1;

    Array<Slot, LEN> _slots;

    static Res<Strong<Domain>> create();

    Domain() : BaseObject(Hj::Type::DOMAIN) {}

    Res<Hj::Cap> add(Hj::Cap dest, Strong<Object> obj);

    Res<Strong<Object>> get(Hj::Cap cap);

    template <typename T>
    Res<Strong<T>> get(Hj::Cap cap) {
        auto obj = try$(get(cap));
        if (not obj.is<T>()) {
            return Error::invalidHandle("type missmatch");
        }
        return Ok(try$(obj.cast<T>()));
    }

    Res<> drop(Hj::Cap cap);
};

struct ObjectLockScope : public LockScope {
    ObjectLockScope(Object &obj)
        : LockScope(obj._lock) {
    }
};

} // namespace Hjert::Core

template <Meta::Derive<Hjert::Core::Object> T>
struct Karm::Fmt::Formatter<T> {
    Res<usize> format(Io::TextWriter &writer, Hjert::Core::Object const &obj) {
        return Fmt::format(writer, "{}({}, '{}')", Fmt::toPascalCase(Hj::toStr(obj._type)).unwrap(), obj.id(), obj.label());
    }
};
