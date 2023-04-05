#pragma once

#include <embed-sys/sys.h>
#include <karm-base/time.h>

namespace Karm::Sys {

inline TimeStamp now() {
    return Embed::now();
}

inline TimeSpan uptime() {
    return Embed::uptime();
}

} // namespace Karm::Sys
