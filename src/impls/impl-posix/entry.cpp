#include <stdio.h>
#include <stdlib.h>

//
#include <karm-base/panic.h>
#include <karm-sys/chan.h>
#include <karm-sys/context.h>

void __panicHandler(Karm::PanicKind kind, char const* msg) {
    fprintf(stderr, "%s: %s\n", kind == Karm::PanicKind::PANIC ? "panic" : "debug", msg);

    if (kind == Karm::PanicKind::PANIC) {
        abort();
        __builtin_unreachable();
    }
}
