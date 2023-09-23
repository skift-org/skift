#pragma once

#include "_embed.h"

#include "time.h"

namespace Karm::Sys {

struct Proc {
};

inline Res<> sleep(TimeSpan span) {
    return _Embed::sleep(span);
}

inline Res<> sleepUntil(TimeStamp stamp) {
    auto n = now();
    if (stamp < n)
        return Ok();
    return sleep(stamp - n);
}

} // namespace Karm::Sys
