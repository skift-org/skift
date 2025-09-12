export module Karm.Core:base.panic;

import :base.base;

namespace Karm {

export enum class PanicKind {
    DEBUG,
    PANIC,
};

using PanicHandler = void (*)(PanicKind kind, char const* msg);

static PanicHandler panicHandler = nullptr;

export void registerPanicHandler(PanicHandler handler) {
    panicHandler = handler;
}

void _panic(PanicKind kind, char const* msg) {
    if (panicHandler)
        panicHandler(kind, msg);
    else
        // Oh no! We don't have a panic handler!
        // Let's just crash the program.
        __builtin_trap();
}

export void debug(char const* msg) {
    _panic(PanicKind::DEBUG, msg);
}

export [[noreturn]] void panic(char const* msg) {
    _panic(PanicKind::PANIC, msg);
    __builtin_unreachable();
}

export [[noreturn]] void notImplemented() {
    panic("not implemented");
}

export [[noreturn]] void unreachable() {
    panic("unreachable");
}

export void breakpoint() {
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

} // namespace Karm
