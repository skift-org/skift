#pragma once

#include <hjert-api/api.h>
#include <karm-base/array.h>
#include <karm-base/lock.h>
#include <karm-base/rc.h>
#include <karm-base/res.h>

namespace Hjert::Core {

struct Object {
    Lock _lock;

    virtual ~Object() = default;
};

template <typename Crtp>
struct BaseObject : public Object {
    using _Crtp = Crtp;
};

using Slot = Opt<Strong<Object>>;

struct Domain : public BaseObject<Domain> {
    Array<Slot, 4096> _slots;

    static Res<Strong<Domain>> create(usize len);

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

} // namespace Hjert::Core
