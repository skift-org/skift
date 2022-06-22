#pragma once

#include <karm-base/result.h>
#include <karm-base/string.h>

namespace Hjert::Arch {

Result<size_t> writeLog(Str str);

[[noreturn]] void stopCpu();

} // namespace Hjert::Arch
