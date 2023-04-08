#include "object.h"

namespace Hjert::Core {

Atomic<usize> Object::_counter = 0;

/* --- Domain --------------------------------------------------------------- */

Res<Strong<Domain>> Domain::create(usize) {
    return Ok(makeStrong<Domain>());
}

Res<> Domain::_set(Hj::Cap cap, Strong<Object> obj) {
    auto i = cap.raw();

    if (i > 0 && i < _slots.len()) {
        _slots[i] = obj;
        return Ok();
    }

    return Error::invalidHandle("invalid slot");
}

Res<Hj::Cap> Domain::add(Hj::Cap dest, Strong<Object> obj) {
    ObjectLockScope scope(*this);

    if (dest.isNone()) {
        for (usize i = 1; i < _slots.len(); i++) {
            if (not _slots[i]) {
                _slots[i] = obj;
                return Ok(i);
            }
        }
        return Error::invalidHandle("no free slots");
    }

    try$(_set(dest, obj));
    return Ok(dest);
}

Res<Strong<Object>> Domain::get(Hj::Cap cap) {
    ObjectLockScope scope(*this);

    auto i = cap.raw();
    if (i > 0 && i < _slots.len()) {
        if (auto slot = _slots[i]) {
            return Ok(*slot);
        }

        return Error::invalidHandle("slot is empty");
    }

    return Error::invalidHandle("invalid slot");
}

Res<> Domain::drop(Hj::Cap cap) {
    ObjectLockScope scope(*this);

    auto i = cap.raw();
    if (i > 0 && i < _slots.len()) {
        _slots[i] = NONE;
        return Ok();
    }

    return Error::invalidHandle("invalid slot");
}

} // namespace Hjert::Core
