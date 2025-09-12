#pragma once

import Karm.Core;

namespace Karm::Logger::_Embed {

void loggerLock();

void loggerUnlock();

Io::TextWriter& loggerOut();

} // namespace Karm::Logger::_Embed
