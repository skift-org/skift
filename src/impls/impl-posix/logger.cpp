#include <karm-logger/_embed.h>
#include <karm-sys/chan.h>

namespace Karm::Logger::_Embed {

void loggerLock() {}

void loggerUnlock() {}

Io::TextWriter& loggerOut() {
    return Sys::err();
}

} // namespace Karm::Logger::_Embed
