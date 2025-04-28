#pragma once

#include "_embed.h"
#include "time.h"

namespace Karm::Sys {

static inline Res<> sleep(Duration span) {
    return _Embed::sleep(span);
}

static inline Res<> sleepUntil(Instant until) {
    return _Embed::sleepUntil(until);
}

static inline Res<Mime::Url> pwd() {
    return _Embed::pwd();
}

// MARK: Sandboxing ------------------------------------------------------------

Res<> enterSandbox();

Res<> ensureUnrestricted();

} // namespace Karm::Sys
