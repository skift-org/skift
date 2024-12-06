#pragma once

namespace Karm {
struct Backtrace;
} // namespace Karm

namespace Karm::_Embed {

// MARK: Backtrace -------------------------------------------------------------

Backtrace captureBacktrace();

Backtrace forceCaptureBacktrace();

// MARK: Locks -----------------------------------------------------------------

void relaxe();

void enterCritical();

void leaveCritical();

} // namespace Karm::_Embed
