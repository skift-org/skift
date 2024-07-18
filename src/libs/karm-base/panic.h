#pragma once

#include "base.h"

namespace Karm {

enum class PanicKind {
    DEBUG,
    PANIC,
};

using PanicHandler = void (*)(PanicKind kind, char const *msg);

void registerPanicHandler(PanicHandler handler);

void _panic(PanicKind kind, char const *msg);

inline void debug(char const *msg) {
    _panic(PanicKind::DEBUG, msg);
}

[[noreturn]] inline void panic(char const *msg) {
    _panic(PanicKind::PANIC, msg);
    __builtin_unreachable();
}

[[noreturn]] inline void notImplemented() {
    panic("not implemented");
}

[[noreturn]] inline void unreachable() {
    panic("unreachable");
}

inline void breakpoint() {
#ifdef __ck_debug__
#    ifdef __clang__
    __builtin_debugtrap();
#    elif defined(__GNUC__)
    __builtin_trap(); // TODO: use __builtin_debugtrap() when it's available
#    elif defined(_MSC_VER)
    __debugbreak();
#    else
#        error "Unsupported compiler"
#    endif
#endif
}

#define assume$(__expr, ...)                                                                                                          \
    if (not(__expr)) [[unlikely]] {                                                                                                   \
        panic(__FILE__ "::" stringify$(__func__) ":" stringify$(__LINE__) ": bad assumption: " #__expr __VA_OPT__(": ") __VA_ARGS__); \
    }

} // namespace Karm
