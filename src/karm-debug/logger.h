#pragma once

#include <karm-fmt/fmt.h>
#include <karm-sys/chan.h>

namespace Karm::Debug {

static inline void log(Str format, auto &&...ts) {
    (void)Fmt::format(Sys::out(), format, std::forward<decltype(ts)>(ts)...);
    (void)Io::putc(Sys::out(), '\n');
}

} // namespace Karm::Debug
