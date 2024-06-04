#pragma once

#include "base.h"

namespace Web::Layout {

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

    void layout(Types::RectPx) override {
    }

    Types::Vec2Px size(Types::Vec2Px) override {
        return {};
    }
};

} // namespace Web::Layout
