#pragma once

#include "base.h"

namespace Web::Layout {

struct TableFlow : public Flow {
    static constexpr auto TYPE = TABLE;

    using Flow::Flow;

    struct Dim {
        usize frag;
    };

    Vec<Dim> _rows;
    Vec<Dim> _cols;

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
