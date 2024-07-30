#pragma once

#include "flow.h"

namespace Vaev::Layout {

struct FlexFlow : public Flow {
    static constexpr auto TYPE = FLEX;

    struct Item {
        usize frag;
    };

    struct Line {
        urange items;
    };

    Vec<Item> _items;
    Vec<Line> _lines;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void _clear();

    void _createItems();

    void _sortByOrder();

    void placeChildren(Context &ctx, Box box) override;

    Px computeIntrinsicSize(Context &ctx, Axis axis, IntrinsicSize intrinsic, Px) override;
};

} // namespace Vaev::Layout
