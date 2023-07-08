#pragma once

#include <karm-base/error.h>
#include <karm-base/res.h>

namespace Posix {

Error fromErrno(isize error);

Error fromLastErrno();

Res<> consumeErrno();

} // namespace Posix
