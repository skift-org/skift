#pragma once

#include "base.h"

namespace Web::Layout {

struct BlockFlow : public Flow {
    static constexpr auto TYPE = BLOCK;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void layout(RectPx) override {
    }

    Vec2Px size(Vec2Px) override {
        return {};
    }
};

} // namespace Web::Layout
