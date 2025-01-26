#pragma once

#include <karm-base/time.h>

#include "_embed.h"

namespace Karm::Sys {

inline SystemTime now() {
    return _Embed::now();
}

inline Instant instant() {
    return _Embed::instant();
}

inline Duration uptime() {
    return _Embed::uptime();
}

inline DateTime dateTime() {
    return DateTime::fromInstant(now());
}

inline Date date() {
    return dateTime().date;
}

inline Time time() {
    return dateTime().time;
}

} // namespace Karm::Sys
