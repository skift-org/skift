#pragma once

#include <karm-base/result.h>
#include <karm-base/string.h>
#include <karm-io/traits.h>

#include "kernel.h"

namespace Hjert::Arch {

Error init();

Io::TextWriter<> &loggerOut();

[[noreturn]] void stopAll();

[[noreturn]] void idleCpu();

} // namespace Hjert::Arch
