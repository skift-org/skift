export module Karm.Gc:root;

import :ptr;

namespace Karm::Gc {

// FIXME: Proper handling of root references
export template <typename T>
using Root = Ref<T>;

} // namespace Karm::Gc
