#pragma once

#include "flow.h"

namespace Vaev::Layout {

struct InlineFlow : public Flow {
    static constexpr auto TYPE = INLINE;

    using Flow::Flow;

    struct Item {
        usize frag;
    };

    struct Line {
    };

    Vec<Item> _items;
    Vec<Line> _lines;

    Type type() const override {
        return TYPE;
    }

    void layout(RectPx bound) override {
        Flow::layout(bound);
    }
};

} // namespace Vaev::Layout
