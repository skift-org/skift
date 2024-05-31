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

    void _clear() {
        _cells.clear();
        _rows.clear();
        _cols.clear();
    }

    void _createCells() {
        for (usize i = 0; i < _frags.len(); i++)
            _cells.pushBack({i});
    }

    void _sortByOrder() {
        stableSort(_cells, [&](auto a, auto b) {
            return style(a.frag).order < style(b.frag).order;
        });
    }

    void layout(Unit::RectPx bound) override {
        _clear();
        _createCells();
        _sortByOrder();

        for (auto &c : _frags) {
            c->layout(bound);
        }
    }

    Unit::Vec2Px size(Unit::Vec2Px) override {
        return {};
    }
};

} // namespace Web::Layout
