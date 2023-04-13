#include "object.h"

namespace Hjert::Core {

/* --- Object --------------------------------------------------------------- */

Atomic<usize> Object::_counter = 0;

/* --- Domain --------------------------------------------------------------- */

Res<Strong<Domain>> Domain::create() {
    return Ok(makeStrong<Domain>());
}

Res<Hj::Cap> Domain::add(Hj::Cap dest, Strong<Object> obj) {
    ObjectLockScope scope(*this);

    auto c = dest.raw();

    if (c != 0)
        return try$(get<Domain>(c & MASK))->add(c >> SHIFT, obj);

    for (c = 1; c < _slots.len(); c++) {
        if (not _slots[c]) {
            _slots[c] = obj;
            return Ok(c);
        }
    }

    return Error::invalidHandle("no free slots");
}

Res<Strong<Object>> Domain::get(Hj::Cap cap) {
    ObjectLockScope scope(*this);

    auto c = cap.raw();

    if (c & ~MASK)
        return try$(get<Domain>(c & MASK))->get(c >> SHIFT);

    if (not _slots[c])
        return Error::invalidHandle("slot is empty");

    return Ok(*_slots[c]);
}

Res<> Domain::drop(Hj::Cap cap) {
    ObjectLockScope scope(*this);

    auto c = cap.raw();

    if (c & ~MASK)
        return try$(get<Domain>(c & MASK))->drop(c >> SHIFT);

    if (not _slots[c])
        return Error::invalidHandle("slot is empty");

    _slots[c] = NONE;
    return Ok();
}

} // namespace Hjert::Core
