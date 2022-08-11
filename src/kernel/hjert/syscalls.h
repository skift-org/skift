#pragma once

#include <hjert-api/types.h>
#include <karm-base/error.h>

namespace Hjert {

Error handleSyscall(Api::Id id, Api::Arg args);

} // namespace Hjert
