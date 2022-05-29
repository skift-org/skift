#pragma once

#include <karm-base/result.h>

namespace Karm::Cli {

struct Success {};
static constexpr Success SUCCESS = {};

using Result = Result<Success>;

} // namespace Karm::Cli
