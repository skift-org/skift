#pragma once

#include "base.h"

namespace Vaev::Layout {

struct BlockFlow : public Flow {
    static constexpr auto TYPE = BLOCK;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void layout(RectPx bound) override {
        Flow::layout(bound);
    }

    Vec2Px size(Vec2Px) override {
        return {};
    }
};

} // namespace Vaev::Layout
