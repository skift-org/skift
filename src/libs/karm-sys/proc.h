#pragma once

#include "_embed.h"

#include "time.h"

namespace Karm::Sys {

inline Res<> sleep(TimeSpan span) {
    return _Embed::sleep(span);
}

inline Res<> sleepUntil(TimeStamp until) {
    return _Embed::sleepUntil(until);
}

} // namespace Karm::Sys
