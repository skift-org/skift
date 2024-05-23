#pragma once

#include "base.h"

namespace Web::Layout {

struct FlexFlow : public Flow {
    static constexpr auto TYPE = FLEX;

    struct Item {
    };

    struct Line {
    };

    Vec<Item> _items;
    Vec<Line> _lines;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void layout(Unit::RectPx) override {
    }

    Unit::Vec2Px size() override {
        return {};
    }
};

} // namespace Web::Layout
