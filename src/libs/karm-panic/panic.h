#pragma once

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

} // namespace Karm
