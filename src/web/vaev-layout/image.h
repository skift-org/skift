#pragma once

#include <karm-media/image.h>

#include "frag.h"

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

    void paint(Paint::Stack &) override {}
};

} // namespace Vaev::Layout
