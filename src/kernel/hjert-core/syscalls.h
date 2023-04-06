#pragma once

#include <hjert-api/api.h>

namespace Hjert::Core {

Res<> doSyscall(Hj::Syscall id, Hj::Args args);

} // namespace Hjert::Core
