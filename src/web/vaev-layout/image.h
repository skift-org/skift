#pragma once

#include <karm-media/image.h>

#include "base.h"

namespace Vaev::Layout {

struct ImageFrag : public Frag {
    static constexpr auto TYPE = IMAGE;

    Media::Image _image;

    ImageFrag(Strong<Style::Computed> style, Media::Image image)
        : Frag(style), _image(image) {
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
