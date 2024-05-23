#pragma once

#include "base.h"

namespace Web::Layout {

struct FlexFlow : public Flow {
    struct Item {
    };

    struct Line {
    };

    Vec<Item> _items;
    Vec<Line> _lines;
};

} // namespace Web::Layout
