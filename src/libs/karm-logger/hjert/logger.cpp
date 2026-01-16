module;

#include <hjert-core/arch.h>

module Karm.Logger;

import Karm.Core;

namespace Karm::Logger::_Embed {

static Lock _lock;

void loggerLock() {
    _lock.acquire();
}

void loggerUnlock() {
    _lock.release();
}

Io::TextWriter& loggerOut() {
    return Hjert::Arch::globalOut();
}

} // namespace Karm::Logger::_Embed
