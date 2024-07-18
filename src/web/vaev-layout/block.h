#pragma once

#include "flow.h"

namespace Vaev::Layout {

struct BlockFlow : public Flow {
    static constexpr auto TYPE = BLOCK;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void placeChildren(RectPx bound) override {
        Frag::placeChildren(bound);

        Px res = Px{bound.top()};
        auto inlineSize = computeWidth();
        for (auto &c : _frags) {
            auto blockSize = c->computeHeight();
            c->placeChildren(RectPx{inlineSize, blockSize}.offset({bound.start(), res}));
            res += blockSize;
        }
    }

    Px computeHeight() override {
        Px res = Px{};
        for (auto &c : _frags)
            res += c->computeHeight();
        return res;
    }

    Px computeWidth() override {
        Px res = Px{};
        for (auto &c : _frags)
            res = max(res, c->computeWidth());
        return res;
    }
};

} // namespace Vaev::Layout
