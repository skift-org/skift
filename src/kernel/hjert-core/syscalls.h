#pragma once

#include <hjert-api/raw.h>

namespace Hjert::Core {

Res<> doSyscall(Hj::Syscall id, Hj::Args args);

} // namespace Hjert::Core
