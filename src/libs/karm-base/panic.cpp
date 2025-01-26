#include "panic.h"

namespace Karm {

static PanicHandler panicHandler = nullptr;

void registerPanicHandler(PanicHandler handler) {
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

} // namespace Karm
