#pragma once

#include <karm-base/error.h>

namespace Embed::Posix {

Error fromErrno(int error);

Error fromLastErrno();

} // namespace Embed::Posix
