#pragma once

#include <karm-media/font.h>

#include "frag.h"

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

    Px contentInlineSize() const override {
        return Px{0};
    }

    Px contentBlockSize() const override {
        return Px{0};
    }

    void paint(Paint::Stack &) override {}
};

} // namespace Vaev::Layout
