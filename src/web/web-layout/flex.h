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

    void _clear() {
        _items.clear();
        _lines.clear();
    }

    void _createItems() {
        for (usize i = 0; i < _frags.len(); i++)
            _items.pushBack({i});
    }

    void _sortByOrder() {
        stableSort(_items, [&](auto a, auto b) {
            return style(a.frag).order < style(b.frag).order;
        });
    }

    void layout(RectPx bound) override {
        _clear();
        _createItems();
        _sortByOrder();

        for (auto &c : _frags) {
            c->layout(bound);
        }
    }

    Vec2Px size(Vec2Px) override {
        return {};
    }
};

} // namespace Web::Layout
