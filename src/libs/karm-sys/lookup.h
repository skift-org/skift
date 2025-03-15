#pragma once

#include <karm-async/task.h>

#include "addr.h"

namespace Karm::Sys {

Async::Task<Vec<Ip>> lookupAsync(Str host);

} // namespace Karm::Sys
