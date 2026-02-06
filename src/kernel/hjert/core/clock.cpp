export module Hjert.Core:clock;

import Karm.Core;

using namespace Karm;

namespace Hjert::Core {

static Lock _lock{};
static Instant _stamp{};

export Instant clockNow() {
    LockScope _{_lock};
    return _stamp;
}

export void clockTick(Duration span) {
    LockScope _{_lock};
    _stamp += span;
}

} // namespace Hjert::Core
