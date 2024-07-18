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
            return styleAt(a.frag).order < styleAt(b.frag).order;
        });
    }

    void placeChildren(RectPx bound) override {
        Frag::placeChildren(bound);

        _clear();
        _createItems();
        _sortByOrder();

        Px res = bound.start();
        auto blockSize = computeHeight();
        for (auto &c : _frags) {
            auto inlineSize = c->computeWidth();
            c->placeChildren(RectPx{inlineSize, blockSize}.offset({res, bound.top()}));
            res += blockSize;
        }
    }

    Px computeHeight() override {
        Px res = Px{};
        for (auto &c : _frags)
            res = max(res, c->computeHeight());
        return res;
    }

    Px computeWidth() override {
        Px res{};
        for (auto &c : _frags)
            res += c->computeWidth();
        return res;
    }
};

} // namespace Vaev::Layout
