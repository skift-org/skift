#pragma once

#include <karm-base/result.h>

namespace Hjert::Arch {

Result<size_t> writeLog(void const *log, size_t size);

[[noreturn]] void stopCpu();

} // namespace Hjert::Arch
