#pragma once

#include "object.h"

namespace Hjert::Core {

struct Domain : public BaseObject<Domain, Hj::Type::DOMAIN> {
    using Slot = Opt<Strong<Object>>;

    Array<Slot, Hj::Cap::LEN> _slots;

    static Res<Strong<Domain>> create();

    Res<Hj::Cap> add(Hj::Cap dest, Strong<Object> obj);

    Res<Strong<Object>> _getUnlock(Hj::Cap cap);

    Res<Strong<Object>> get(Hj::Cap cap);

    template <typename T>
    Res<Strong<T>> _getUnlock(Hj::Cap cap) {
        auto obj = try$(_getUnlock(cap));
        if (not obj.is<T>())
            return Error::invalidHandle("type missmatch");
        return Ok(try$(obj.cast<T>()));
    }

    template <typename T>
    Res<Strong<T>> get(Hj::Cap cap) {
        ObjectLockScope scope(*this);
        return _getUnlock<T>(cap);
    }

    Res<> drop(Hj::Cap cap);
};

} // namespace Hjert::Core
