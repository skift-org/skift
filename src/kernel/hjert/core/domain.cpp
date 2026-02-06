module;

#include <karm/macros>

export module Hjert.Core:domain;

import Karm.Core;
import Hjert.Api;
import :object;

using namespace Karm;

namespace Hjert::Core {

struct Domain : BaseObject<Domain, Hj::Type::DOMAIN> {
    using Slot = Opt<Arc<Object>>;

    Array<Slot, Hj::Cap::LEN> _slots;

    static Res<Arc<Domain>> create() {
        return Ok(makeArc<Domain>());
    }

    Res<Hj::Cap> _addUnlock(Hj::Cap dest, Arc<Object> obj) {
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

    Res<Hj::Cap> add(Hj::Cap dest, Arc<Object> obj) {
        ObjectLockScope scope(*this);
        return _addUnlock(dest, obj);
    }

    Res<Arc<Object>> _getUnlock(Hj::Cap cap) {
        auto c = cap.raw();

        if (c & ~Hj::Cap::MASK) {
            auto subDomain = try$(_getUnlock<Domain>(Hj::Cap{c & Hj::Cap::MASK}));
            return subDomain->get(Hj::Cap{c >> Hj::Cap::SHIFT});
        }

        if (not _slots[c])
            return Error::invalidHandle("slot is empty");

        return Ok(*_slots[c]);
    }

    Res<Arc<Object>> get(Hj::Cap cap) {
        ObjectLockScope scope(*this);
        return _getUnlock(cap);
    }

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

    Res<> drop(Hj::Cap cap) {
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

    usize _availableUnlocked() const {
        usize count = 0;

        for (usize i = 1; i < _slots.len(); i++) {
            if (_slots[i])
                count++;
        }

        return count;
    }
};

} // namespace Hjert::Core
