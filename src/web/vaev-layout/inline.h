#pragma once

#include "flow.h"

namespace Vaev::Layout {

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

    void placeChildren(RectPx bound) override {
        Frag::placeChildren(bound);

        Px res = bound.start();
        for (auto &c : _frags) {
            auto inlineSize = c->computeWidth();
            auto blockSize = c->computeHeight();
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
