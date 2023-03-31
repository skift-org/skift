#pragma once

#include <embed-sys/sys.h>

#include "time.h"

namespace Karm::Sys {

struct Proc {
};

inline Res<> sleep(TimeSpan span) {
    return Embed::sleep(span);
}

inline Res<> sleepUntil(TimeStamp stamp) {
    return sleep(stamp - now());
}

} // namespace Karm::Sys
