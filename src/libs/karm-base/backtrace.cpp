#include "backtrace.h"

#include "_embed.h"

namespace Karm {

Backtrace Backtrace::capture() {
    return _Embed::captureBacktrace();
}

Backtrace Backtrace::forceCapture() {
    return _Embed::forceCaptureBacktrace();
}

} // namespace Karm
