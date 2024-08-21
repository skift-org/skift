#pragma once

#include <vaev-base/length.h>
#include <vaev-base/writing.h>

namespace Vaev::Layout {

enum struct IntrinsicSize {
    AUTO,
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

enum struct Commit {
    NO,  // No, only compute sizes
    YES, // Yes, commit computed values to the tree
};

struct Input {
    Commit commit = Commit::NO; //< Should the computed values be committed to the DOM?
    Axis axis = Axis::HORIZONTAL;
    IntrinsicSize intrinsic = IntrinsicSize::AUTO;
    Vec2Px availableSpace = {};
    RectPx containingBlock{};
};

struct Output {
    Vec2Px size;

    static Output fromSize(Vec2Px size) {
        return Output{size};
    }
};

struct Frag;

struct Tree;

} // namespace Vaev::Layout
