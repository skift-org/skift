#pragma once

#include "_embed.h"
#include "time.h"

namespace Karm::Sys {

inline Res<> sleep(Duration span) {
    return _Embed::sleep(span);
}

inline Res<> sleepUntil(Instant until) {
    return _Embed::sleepUntil(until);
}

// MARK: Sandboxing ------------------------------------------------------------

void enterSandbox();

Res<> ensureUnrestricted();

} // namespace Karm::Sys
