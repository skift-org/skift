#pragma once

#include "base.h"

namespace Web::Layout {

struct BlockFlow : public Flow {
    static constexpr auto TYPE = BLOCK;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void layout(Types::RectPx) override {
    }

    Types::Vec2Px size(Types::Vec2Px) override {
        return {};
    }
};

} // namespace Web::Layout
