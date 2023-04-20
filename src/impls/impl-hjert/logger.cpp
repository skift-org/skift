#include <embed-logger/logger.h>
#include <hjert-core/arch.h>
#include <karm-base/lock.h>
#include <karm-sys/chan.h>

namespace Embed {

static Lock _lock;

void loggerLock() {
    _lock.acquire();
}

void loggerUnlock() {
    _lock.release();
}

Io::TextWriter &loggerOut() {
    return Hjert::Arch::loggerOut();
}

} // namespace Embed
