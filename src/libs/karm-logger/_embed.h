#pragma once

#include <karm-io/traits.h>

namespace Karm::Logger::_Embed {

void loggerLock();

void loggerUnlock();

Io::TextWriter& loggerOut();

} // namespace Karm::Logger::_Embed
