#pragma once

#include <karm-base/error.h>

namespace Karm::Embed::Posix {

Base::Error fromErrno(int error);

Base::Error fromLastErrno();

} // namespace Karm::Embed::Posix
