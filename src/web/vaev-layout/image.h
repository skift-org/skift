#pragma once

#include <karm-image/picture.h>

#include "frag.h"

namespace Vaev::Layout {

struct ImageFrag : public Frag {
    static constexpr auto TYPE = IMAGE;

    Image::Picture _image;

    ImageFrag(Strong<Style::Computed> style, Image::Picture image)
        : Frag(style), _image(image) {
    }

    Type type() const override {
        return TYPE;
    }

    void makePaintables(Paint::Stack &) override {}
};

} // namespace Vaev::Layout
