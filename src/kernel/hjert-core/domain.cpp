#include "domain.h"

namespace Hjert::Core {

Res<Arc<Domain>> Domain::create() {
    return Ok(makeArc<Domain>());
}

Res<Hj::Cap> Domain::_addUnlock(Hj::Cap dest, Arc<Object> obj) {
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

Res<Hj::Cap> Domain::add(Hj::Cap dest, Arc<Object> obj) {
    ObjectLockScope scope(*this);
    return _addUnlock(dest, obj);
}

Res<Arc<Object>> Domain::_getUnlock(Hj::Cap cap) {
    auto c = cap.raw();

    if (c & ~Hj::Cap::MASK) {
        auto subDomain = try$(_getUnlock<Domain>(Hj::Cap{c & Hj::Cap::MASK}));
        return subDomain->get(Hj::Cap{c >> Hj::Cap::SHIFT});
    }

    if (not _slots[c])
        return Error::invalidHandle("slot is empty");

    return Ok(*_slots[c]);
}

Res<Arc<Object>> Domain::get(Hj::Cap cap) {
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

usize Domain::_availableUnlocked() const {
    usize count = 0;

    for (usize i = 1; i < _slots.len(); i++) {
        if (_slots[i])
            count++;
    }

    return count;
}

} // namespace Hjert::Core
