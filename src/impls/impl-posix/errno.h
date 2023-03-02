#pragma once

#include <karm-base/error.h>

namespace Embed::Posix {

Error fromErrno(isize error);

Error fromLastErrno();

} // namespace Embed::Posix
