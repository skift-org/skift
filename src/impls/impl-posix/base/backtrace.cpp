#include <execinfo.h>
#include <karm-base/_embed.h>
#include <karm-base/backtrace.h>

namespace Karm::_Embed {

Backtrace captureBacktrace() {
    return forceCaptureBacktrace();
}

Backtrace forceCaptureBacktrace() {
    Backtrace bt = Backtrace::CAPTURED;
    void* buffer[64];
    int count = backtrace(buffer, 64);
    char** symbols = backtrace_symbols(buffer, count);

    for (int i = 0; i < count; i++) {
        char* symbol = symbols[i];
        bt._frames.pushBack({
            String(symbol),
            String("unknown"),
            0,
        });
    }

    free(symbols);
    return bt;
}

} // namespace Karm::_Embed
