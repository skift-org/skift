#pragma once

#include <karm-media/font.h>

#include "base.h"

namespace Web::Layout {

struct Run : public Frag {
    static constexpr auto TYPE = RUN;

    Run(Strong<Cssom::Style> style, String)
        : Frag(style) {
    }

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
