#include <execinfo.h>
#include <karm-base/backtrace.h>
#include <karm-io/aton.h>
#include <stdio.h>
#include <stdlib.h>

#include <karm-base/_embed.h>

namespace Karm::_Embed {

// MARK: Backtrace -------------------------------------------------------------

Backtrace captureBacktrace() {
    return forceCaptureBacktrace();
}

Backtrace forceCaptureBacktrace() {
    Backtrace bt = Backtrace::CAPTURED;
    void *buffer[64];
    int count = backtrace(buffer, 64);
    char **symbols = backtrace_symbols(buffer, count);

    for (int i = 0; i < count; i++) {
        char *symbol = symbols[i];
        bt._frames.pushBack({
            String(symbol),
            String("unknown"),
            0,
        });
    }

    free(symbols);
    return bt;
}

// MARK: Locks -----------------------------------------------------------------

void relaxe() {
#if defined(__x86_64__)
    asm volatile("pause");
#endif
}

void enterCritical() {
    // NOTE: We don't do any thread so we don't need to do anything here.
}

void leaveCritical() {
    // NOTE: We don't do any thread so we don't need to do anything here.
}

} // namespace Karm::_Embed
