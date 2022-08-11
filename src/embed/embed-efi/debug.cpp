#include <embed/debug.h>
#include <karm-sys/chan.h>

namespace Embed {

void loggerLock() {}

void loggerUnlock() {}

Io::TextWriter<Encoding> &loggerOut() {
    return Sys::err();
}

} // namespace Embed
