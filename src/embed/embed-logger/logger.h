#pragma once

#include <karm-io/traits.h>

namespace Embed {

void loggerLock();

void loggerUnlock();

Io::TextWriter &loggerOut();

} // namespace Embed
