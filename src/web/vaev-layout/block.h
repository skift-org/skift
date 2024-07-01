#pragma once

#include "flow.h"

namespace Vaev::Layout {

struct BlockFlow : public Flow {
    static constexpr auto TYPE = BLOCK;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void layout(RectPx bound) override {
        Frag::layout(bound);

        Px res = Px{0};
        for (auto &c : _frags) {
            auto inlineSize = c->contentInlineSize();
            auto blockSize = c->contentBlockSize();
            c->layout(RectPx{inlineSize, blockSize}.offset({Px{0}, res}));
            res += blockSize;
        }
    }

    Px contentBlockSize() const override {
        Px res = Px{};
        for (auto &c : _frags)
            res += c->contentBlockSize();
        return max(res, Px{100});
    }
};

} // namespace Vaev::Layout
