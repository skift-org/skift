#pragma once

#include <embed/debug.h>

namespace Karm {

[[noreturn]] static inline void panic(char const *message) {
    Embed::panicHandler(message);
}

[[noreturn]] static inline void notImplemented() {
    panic("Not implemented");
}

} // namespace Karm
