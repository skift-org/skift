#pragma once

#include <karm-math/vec.h>
import Karm.Sys;

namespace Strata::Shell::Api {

using Instance = usize;

struct CreateInstance {
    using Response = Instance;
    Math::Vec2i size;
};

} // namespace Strata::Shell::Api
