#pragma once

#include "base.h"

namespace Web::Layout {

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

    void layout(Unit::RectPx) override {
        // sort cells by order
        stableSort(_items, [&](auto a, auto b) {
            return style(a.frag).order < style(b.frag).order;
        });
    }

    Unit::Vec2Px size() override {
        return {};
    }
};

} // namespace Web::Layout
