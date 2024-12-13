#pragma once

#include "box.h"

namespace Vaev::Layout {

struct Metrics {
    InsetsPx padding{};
    InsetsPx borders{};
    Vec2Px position; //< Position relative to the content box of the containing block
    Vec2Px borderSize;
    InsetsPx margin{};
    RadiiPx radii{};

    void repr(Io::Emit &e) const {
        e("(layout paddings: {} borders: {} position: {} borderSize: {} margin: {} radii: {})",
          padding, borders, position, borderSize, margin, radii);
    }

    RectPx borderBox() const {
        return RectPx{position, borderSize};
    }

    RectPx paddingBox() const {
        return borderBox().shrink(borders);
    }

    RectPx contentBox() const {
        return paddingBox().shrink(padding);
    }

    RectPx marginBox() const {
        return borderBox().grow(margin);
    }
};

struct Frag {
    MutCursor<Box> box;
    Metrics metrics;
    Vec<Frag> children;

    Frag(MutCursor<Box> box) : box{std::move(box)} {}

    Frag() : box{nullptr} {}

    Style::Computed const &style() const {
        return *box->style;
    }

    /// Offset the position of this fragment and its subtree.
    void offset(Vec2Px d) {
        metrics.position = metrics.position + d;
        for (auto &c : children)
            c.offset(d);
    }

    /// Add a child fragment.
    void add(Frag &&frag) {
        children.pushBack(std::move(frag));
    }
};

} // namespace Vaev::Layout
