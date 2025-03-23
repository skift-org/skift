#pragma once

#include <karm-math/vec.h>
#include <karm-rpc/base.h>

namespace Strata::Shell::Api {

using Instance = usize;

struct CreateInstance {
    using Response = Instance;
    Math::Vec2i size;
};

} // namespace Strata::Shell::Api
