#pragma once

#include <hjert-api/syscalls.h>
#include <karm-base/error.h>

namespace Hjert {

Error handleSyscall(Api::SyscallId id, Api::Arg args);

} // namespace Hjert
