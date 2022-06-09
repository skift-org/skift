#pragma once

#include <embed/debug.h>

namespace Karm {

static inline void debug(const char* msg){
    Embed::debugHandler(msg);
}

[[noreturn]] static inline void panic(char const *msg) {
    Embed::panicHandler(msg);
}

[[noreturn]] static inline void notImplemented() {
    panic("Not implemented");
}

} // namespace Karm
