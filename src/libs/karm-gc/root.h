#pragma once

#include "ptr.h"

namespace Karm::Gc {

// FIXME: Proper handling of root references
template <typename T>
using Root = Ref<T>;

} // namespace Karm::Gc
