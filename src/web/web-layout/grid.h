#pragma once

#include "base.h"

namespace Web::Layout {

struct GridFlow : public Flow {
    static constexpr auto TYPE = GRID;

    using Flow::Flow;

    struct Cell {
        usize frag;
    };

    struct Dim {
    };

    Vec<Cell> _cells;
    Vec<Dim> _rows;
    Vec<Dim> _cols;

    Type type() const override {
        return TYPE;
    }

    void layout(Unit::RectPx) override {
        // sort cells by order
        stableSort(_cells, [&](auto a, auto b) {
            return style(a.frag).order < style(b.frag).order;
        });
    }

    Unit::Vec2Px size() override {
        return {};
    }
};

} // namespace Web::Layout
