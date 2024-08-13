#pragma once

#include <vaev-base/length.h>

namespace Vaev::Layout {

enum struct IntrinsicSize {
    MIN_CONTENT,
    MAX_CONTENT,
};

struct Viewport {
    Px dpi = Px{96};
    // https://drafts.csswg.org/css-values/#small-viewport-size
    RectPx small;
    // https://drafts.csswg.org/css-values/#large-viewport-size
    RectPx large = small;
    // https://drafts.csswg.org/css-values/#dynamic-viewport-size
    RectPx dynamic = small;
};

} // namespace Vaev::Layout
