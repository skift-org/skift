#pragma once

#include <karm-base/time.h>

#include "_embed.h"

namespace Karm::Sys {

inline TimeStamp now() {
    return _Embed::now();
}

inline TimeSpan uptime() {
    return _Embed::uptime();
}

inline DateTime dateTime() {
    return DateTime::fromTimeStamp(now());
}

inline Date date() {
    return dateTime().date;
}

inline Time time() {
    return dateTime().time;
}

} // namespace Karm::Sys
