#pragma once

#include <karm-base/_embed.h>

namespace Karm {

inline void debug(char const *msg) {
    _Embed::debug(msg);
}

[[noreturn]] inline void panic(char const *msg) {
    _Embed::panic(msg);
}

[[noreturn]] inline void notImplemented() {
    panic("Not implemented");
}

[[noreturn]] inline void unreachable() {
    panic("Unreachable");
}

} // namespace Karm
