#pragma once

#include <karm-base/res.h>

#include "event.h"

namespace Karm::App {

// MARK: Events -----------------------------------------------------------------

struct RequestExitEvent {
    Res<> res = Ok();
};

struct RequestMinimizeEvent {
};

struct RequestMaximizeEvent {
};

// MARK: Host ------------------------------------------------------------------

} // namespace Karm::App
