#pragma once

#include "base.h"

namespace Web::Layout {

struct TextFrag : public Frag {
    static constexpr auto TYPE = TEXT;
    String _text;

    TextFrag(Strong<Css::ComputedValues> style, String text)
        : Frag(style), _text(text) {
    }

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
