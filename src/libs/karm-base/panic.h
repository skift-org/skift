#pragma once

#include <embed-base/base.h>

namespace Karm {

inline void debug(const char *msg) {
    Embed::debug(msg);
}

[[noreturn]] inline void panic(char const *msg) {
    Embed::panic(msg);
}

[[noreturn]] inline void notImplemented() {
    panic("Not implemented");
}

[[noreturn]] inline void unreachable() {
    panic("Unreachable");
}

} // namespace Karm
