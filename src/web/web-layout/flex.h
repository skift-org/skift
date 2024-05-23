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

    Type type() const override {
        return TYPE;
    }
};

} // namespace Web::Layout
