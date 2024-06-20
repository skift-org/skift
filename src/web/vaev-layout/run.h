#pragma once

#include <karm-media/font.h>

#include "base.h"

namespace Vaev::Layout {

struct Run : public Frag {
    static constexpr auto TYPE = RUN;

    Run(Strong<Style::Computed> style, String)
        : Frag(style) {
    }

    Type type() const override {
        return TYPE;
    }

    void layout(RectPx) override {
    }

    Vec2Px size(Vec2Px) override {
        return {};
    }
};

} // namespace Vaev::Layout
