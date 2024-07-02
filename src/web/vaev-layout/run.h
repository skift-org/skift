#pragma once

#include <karm-media/font.h>

#include "frag.h"

namespace Vaev::Layout {

struct Run : public Frag {
    static constexpr auto TYPE = RUN;
    String _text;

    Run(Strong<Style::Computed> style, String text)
        : Frag(style), _text(text) {
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

    void repr(Io::Emit &e) const override {
        e("({} {} {#})", type(), _borderBox, _text);
    }
};

} // namespace Vaev::Layout
