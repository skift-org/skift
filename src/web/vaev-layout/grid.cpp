module;

#include <karm-base/rc.h>

export module Vaev.Layout:grid;

import :block;

namespace Vaev::Layout {

export Rc<FormatingContext> constructGridFormatingContext(Box& box) {
    return constructBlockFormatingContext(box);
}

} // namespace Vaev::Layout
