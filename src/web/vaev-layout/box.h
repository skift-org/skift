#pragma once

#include <vaev-base/sizing.h>

namespace Vaev::Layout {

struct Box {
    SpacingPx paddings{};
    SpacingPx borders{};
    RectPx borderBox{};
    SpacingPx margins{};
    RadiusPx radius{};

    void repr(Io::Emit &e) const {
        e("(box paddings: {} borders: {} borderBox: {} margins: {} radius: {})",
          paddings, borders, borderBox, margins, radius);
    }
};

} // namespace Vaev::Layout
