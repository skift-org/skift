#pragma once

#include "base.h"

namespace Web::Layout {

struct GridFlow : public Flow {
    static constexpr auto TYPE = GRID;

    using Flow::Flow;

    Type type() const override {
        return TYPE;
    }

    void layout(Unit::RectPx) override {
    }

    Unit::Vec2Px size() override {
        return {};
    }
};

} // namespace Web::Layout
