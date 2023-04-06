#include "object.h"

namespace Hjert::Core {

/* --- Domain --------------------------------------------------------------- */

Res<Strong<Domain>> Domain::create(usize) {
    return Ok(makeStrong<Domain>());
}

Res<Hj::Cap> Domain::add(Hj::Cap dest, Strong<Object> obj) {
    ObjectLockScope scope(*this);

    _slots[dest._raw] = obj;
    return Ok(dest);
}

Res<Strong<Object>> Domain::get(Hj::Cap cap) {
    ObjectLockScope scope(*this);

    auto slot = _slots[cap._raw];
    if (!slot)
        return Error::invalidHandle();

    return Ok(*slot);
}

Res<> Domain::drop(Hj::Cap cap) {
    ObjectLockScope scope(*this);

    _slots[cap._raw] = NONE;
    return Ok();
}

} // namespace Hjert::Core
