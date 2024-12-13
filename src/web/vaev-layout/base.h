#pragma once

#include <vaev-base/length.h>
#include <vaev-base/resolution.h>
#include <vaev-base/writing.h>

namespace Vaev::Layout {

enum struct IntrinsicSize {
    AUTO,
    MIN_CONTENT,
    MAX_CONTENT,
    STRETCH_TO_FIT,
};

struct Viewport {
    Resolution dpi = Resolution::fromDpi(96);
    // https://drafts.csswg.org/css-values/#small-viewport-size
    RectPx small;
    // https://drafts.csswg.org/css-values/#large-viewport-size
    RectPx large = small;
    // https://drafts.csswg.org/css-values/#dynamic-viewport-size
    RectPx dynamic = small;
};

struct Box;

struct Tree;

} // namespace Vaev::Layout
