export module Vaev.Engine:layout.grid;

import Karm.Core;
import :layout.block;

using namespace Karm;

namespace Vaev::Layout {

export Rc<FormatingContext> constructGridFormatingContext(Box& box) {
    return constructBlockFormatingContext(box);
}

} // namespace Vaev::Layout
