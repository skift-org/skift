#include <embed/logger.h>
#include <karm-sys/chan.h>

namespace Embed {

void loggerLock() {}

void loggerUnlock() {}

Io::TextWriter<> &loggerOut() {
    return Sys::err();
}

} // namespace Embed
