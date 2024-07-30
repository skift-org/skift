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

    RectPx paddingBox() const {
        return borders.shrink(Math::Flow::LEFT_TO_RIGHT, borderBox);
    }

    RectPx contentBox() const {
        return paddings.shrink(Math::Flow::LEFT_TO_RIGHT, paddingBox());
    }

    RectPx marginBox() const {
        return margins.grow(Math::Flow::LEFT_TO_RIGHT, borderBox);
    }
};

} // namespace Vaev::Layout
