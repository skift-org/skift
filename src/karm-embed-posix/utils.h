#pragma once

#include <karm-base/error.h>

namespace Karm::Embed::Posix {

Error fromErrno(int error);

Error fromLastErrno();

} // namespace Karm::Embed::Posix
