#pragma once

import Karm.Core;

#include "object.h"

namespace Hjert::Core {

struct Listener :
    public BaseObject<Listener, Hj::Type::LISTENER> {

    struct Listened {
        Hj::Cap cap;
        Arc<Object> obj;

        Flags<Hj::Sigs> set;
        Flags<Hj::Sigs> unset;
    };

    Vec<Listened> _listened;
    Vec<Hj::Event> _events;

    static Res<Arc<Listener>> create();

    Res<> listen(Hj::Cap cap, Arc<Object> obj, Flags<Hj::Sigs> set, Flags<Hj::Sigs> unset);

    Slice<Hj::Event> pollEvents();

    Slice<Hj::Event> events() {
        return _events;
    }

    void flush() {
        _events.clear();
    }
};

} // namespace Hjert::Core
