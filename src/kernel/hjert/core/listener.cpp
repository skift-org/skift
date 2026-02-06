export module Hjert.Core:listener;

import Karm.Core;
import :object;

namespace Hjert::Core {

export struct Listener : BaseObject<Listener, Hj::Type::LISTENER> {
    struct Listened {
        Hj::Cap cap;
        Arc<Object> obj;

        Flags<Hj::Sigs> set;
        Flags<Hj::Sigs> unset;
    };

    Vec<Listened> _listened;
    Vec<Hj::Event> _events;

    static Res<Arc<Listener>> create() {
        return Ok(makeArc<Listener>());
    }

    Res<> listen(Hj::Cap cap, Arc<Object> obj, Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset) {
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

    Slice<Hj::Event> pollEvents() {
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

    Slice<Hj::Event> events() {
        return _events;
    }

    void flush() {
        _events.clear();
    }
};

} // namespace Hjert::Core
