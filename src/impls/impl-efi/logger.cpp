#include <karm-logger/_embed.h>

import Karm.Sys;

namespace Karm::Logger::_Embed {

void loggerLock() {}

void loggerUnlock() {}

Io::TextWriter& loggerOut() {
    return Sys::err();
}

} // namespace Karm::Logger::_Embed
