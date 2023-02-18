#pragma once

#include <hjert-api/api.h>

namespace Hjert::Core {

Hj::Code dispatchSyscall(Hj::Syscall id, Hj::Args args);

} // namespace Hjert::Core
