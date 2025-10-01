#include "listener.h"

namespace Hjert::Core {

Res<Arc<Listener>> Listener::create() {
    return Ok(makeArc<Listener>());
}

Res<> Listener::listen(Hj::Cap cap, Arc<Object> obj, Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset) {
    ObjectLockScope _{*this};

    for (usize i = 0; i < _listened.len(); ++i) {
        if (_listened[i].cap == cap) {
            auto& listened = _listened[i];
            listened.set = set;
            listened.unset = unset;

            if (listened.set.empty() and
                listened.unset.empty()) {
                _listened.removeAt(i);
                return Ok();
            }
        }
    }

    _listened.pushBack(Listened{cap, obj, set, unset});
    return Ok();
}

Slice<Hj::Event> Listener::pollEvents() {
    ObjectLockScope _{*this};
    _events.clear();

    for (auto& l : _listened) {
        auto sigs = l.obj->poll();
        if (sigs & l.set) {
            _events.pushBack(Hj::Event{l.cap, sigs & l.set, true});
        }

        if (~sigs & l.unset) {
            _events.pushBack(Hj::Event{l.cap, sigs & l.unset, false});
        }
    }

    return _events;
}

} // namespace Hjert::Core
