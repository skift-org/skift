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
