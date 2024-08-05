#pragma once

#include <vaev-base/sizing.h>

namespace Vaev::Layout {

struct Box {
    InsetsPx paddings{};
    InsetsPx borders{};
    RectPx borderBox{};
    InsetsPx margins{};
    RadiiPx radii{};

    void repr(Io::Emit &e) const {
        e("(box paddings: {} borders: {} borderBox: {} margins: {} radii: {})",
          paddings, borders, borderBox, margins, radii);
    }

    RectPx paddingBox() const {
        return borderBox.shrink(borders);
    }

    RectPx contentBox() const {
        return paddingBox().shrink(paddings);
    }

    RectPx marginBox() const {
        return borderBox.grow(margins);
    }
};

} // namespace Vaev::Layout
