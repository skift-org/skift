#pragma once

#include <embed/base.h>

namespace Karm {

static inline void debug(const char *msg) {
    Embed::debug(msg);
}

[[noreturn]] static inline void panic(char const *msg) {
    Embed::panic(msg);
}

[[noreturn]] static inline void notImplemented() {
    panic("Not implemented");
}

[[noreturn]] static inline void unreachable() {
    panic("Unreachable");
}

} // namespace Karm
