#pragma once

#include "object.h"

namespace Hjert::Core {

struct Domain : BaseObject<Domain, Hj::Type::DOMAIN> {
    using Slot = Opt<Arc<Object>>;

    Array<Slot, Hj::Cap::LEN> _slots;

    static Res<Arc<Domain>> create();

    Res<Hj::Cap> _addUnlock(Hj::Cap dest, Arc<Object> obj);

    Res<Hj::Cap> add(Hj::Cap dest, Arc<Object> obj);

    Res<Arc<Object>> _getUnlock(Hj::Cap cap);

    Res<Arc<Object>> get(Hj::Cap cap);

    template <typename T>
    Res<Arc<T>> _getUnlock(Hj::Cap cap) {
        auto obj = try$(_getUnlock(cap));
        if (not obj.is<T>())
            return Error::invalidHandle("type missmatch");
        return Ok(try$(obj.cast<T>()));
    }

    template <typename T>
    Res<Arc<T>> get(Hj::Cap cap) {
        ObjectLockScope scope(*this);
        return _getUnlock<T>(cap);
    }

    Res<> drop(Hj::Cap cap);

    usize _availableUnlocked() const;
};

} // namespace Hjert::Core
