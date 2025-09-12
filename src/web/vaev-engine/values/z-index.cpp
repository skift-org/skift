export module Vaev.Engine:values.z_index;

import Karm.Core;

import :values.keywords;
import :values.primitives;

using namespace Karm;

namespace Vaev {

export using ZIndex = FlatUnion<
    Keywords::Auto,
    Integer>;

} // namespace Vaev
