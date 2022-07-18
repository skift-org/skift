#pragma once

#include <karm-base/result.h>
#include <karm-base/string.h>
#include <karm-io/traits.h>

namespace Hjert::Arch {

Error init();

Io::TextWriter<Embed::Encoding> &loggerOut();

[[noreturn]] void stopCpu();

} // namespace Hjert::Arch
