#pragma once

#include <karm-media/font.h>

#include "base.h"

namespace Web::Layout {

struct Run : public Frag {
    static constexpr auto TYPE = RUN;

    Run(Strong<Css::ComputedValues> style, String)
        : Frag(style) {
    }

    Type type() const override {
        return TYPE;
    }

    void layout(Unit::RectPx) override {
    }

    Unit::Vec2Px size(Unit::Vec2Px) override {
        return {};
    }
};

} // namespace Web::Layout
