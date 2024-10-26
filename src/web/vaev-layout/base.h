#pragma once

#include <vaev-base/length.h>
#include <vaev-base/writing.h>

namespace Vaev::Layout {

enum struct IntrinsicSize {
    AUTO,
    MIN_CONTENT,
    MAX_CONTENT,
    STRETCH_TO_FIT,
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

enum struct Commit {
    NO,  // No, only compute sizes
    YES, // Yes, commit computed values to the tree
};

/// Input to the layout algorithm.

struct Input {
    Commit commit = Commit::NO; //< Should the computed values be committed to the layout?
    Math::Vec2<IntrinsicSize> intrinsic = {IntrinsicSize::AUTO, IntrinsicSize::AUTO};
    Math::Vec2<Opt<Px>> knownSize = {};
    Vec2Px position = {};
    Vec2Px availableSpace = {};
    Vec2Px containingBlock = {};
};

/// Output of the layout algorithm.

struct Output {
    Vec2Px size;

    static Output fromSize(Vec2Px size) {
        return Output{size};
    }
};

/// Computed layout values.

struct Layout {
    InsetsPx padding{};
    InsetsPx borders{};
    Vec2Px position; //< Position relative to the content box of the containing block
    Vec2Px borderSize;
    InsetsPx margin{};
    RadiiPx radii{};
    Px fontSize{16};

    void repr(Io::Emit &e) const {
        e("(layout paddings: {} borders: {} position: {} borderSize: {} margin: {} radii: {})",
          padding, borders, position, borderSize, margin, radii);
    }

    Layout offseted(Vec2Px offset) const {
        auto copy = *this;
        copy.position = position + offset;
        return copy;
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

struct Frag;

struct Tree;

} // namespace Vaev::Layout
