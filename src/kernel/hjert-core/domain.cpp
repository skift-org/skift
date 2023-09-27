#include "domain.h"

namespace Hjert::Core {

Res<Strong<Domain>> Domain::create() {
    return Ok(makeStrong<Domain>());
}

Res<Hj::Cap> Domain::add(Hj::Cap dest, Strong<Object> obj) {
    ObjectLockScope scope(*this);

    auto c = dest.raw();

    if (c != 0) {
        auto subDomain = try$(_getUnlock<Domain>(Hj::Cap{c & Hj::Cap::MASK}));
        auto newCap = try$(subDomain->add(Hj::Cap{c >> Hj::Cap::SHIFT}, obj));
        return Ok(Hj::Cap{newCap._raw | (c & ~Hj::Cap::MASK)});
    }

    for (c = 1; c < _slots.len(); c++) {
        if (not _slots[c]) {
            _slots[c] = obj;
            return Ok(Hj::Cap{c});
        }
    }

    return Error::invalidHandle("no free slots");
}

Res<Strong<Object>> Domain::_getUnlock(Hj::Cap cap) {
    auto c = cap.raw();

    if (c & ~Hj::Cap::MASK) {
        auto subDomain = try$(_getUnlock<Domain>(Hj::Cap{c & Hj::Cap::MASK}));
        return subDomain->get(Hj::Cap{c >> Hj::Cap::SHIFT});
    }

    if (not _slots[c])
        return Error::invalidHandle("slot is empty");

    return Ok(*_slots[c]);
}

Res<Strong<Object>> Domain::get(Hj::Cap cap) {
    ObjectLockScope scope(*this);
    return _getUnlock(cap);
}

Res<> Domain::drop(Hj::Cap cap) {
    ObjectLockScope scope(*this);

    auto c = cap.raw();

    if (c & ~Hj::Cap::MASK) {
        auto subDomain = try$(_getUnlock<Domain>(Hj::Cap{c & Hj::Cap::MASK}));
        return subDomain->drop(Hj::Cap{c >> Hj::Cap::SHIFT});
    }

    if (not _slots[c])
        return Error::invalidHandle("slot is empty");

    _slots[c] = NONE;
    return Ok();
}

} // namespace Hjert::Core
