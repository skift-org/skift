#pragma once

#include <hjert-api/types.h>

namespace Hjert::Core {

Res<> doSyscall(Hj::Syscall id, Hj::Args args);

} // namespace Hjert::Core
