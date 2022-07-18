#include <embed/debug.h>
#include <hjert/arch.h>
#include <karm-sys/chan.h>

namespace Embed {

void loggerLock() {}

void loggerUnlock() {}

Io::TextWriter<Encoding> &loggerOut() {
    return Hjert::Arch::loggerOut();
}

} // namespace Embed
