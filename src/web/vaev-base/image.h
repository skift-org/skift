#pragma once

#include <karm-mime/url.h>

#include "angle.h"
#include "color.h"
#include "percent.h"

// https://www.w3.org/TR/css-images-4/

namespace Vaev {

struct Image;

// MARK: Linear Gradient -------------------------------------------------------
// https://www.w3.org/TR/css-images-4/#linear-gradients

struct LinearGradient {
    struct Stop {
        Gfx::Color color;
        Percent position;

        void repr(Io::Emit& e) const {
            e("({} {})", color, position);
        }
    };

    Angle angle;
    ColorSpace colorSpace = ColorSpace::SRGB;
    Vec<Stop> stops;

    void repr(Io::Emit& e) const {
        e("(linear-gradient {} {} {}", angle, colorSpace, stops);
    }
};

// MARK: Radial Gradient -------------------------------------------------------
// https://www.w3.org/TR/css-images-4/#radial-gradients

struct RadialGradient {
    enum struct Shape {
        CIRCLE,
        ELLIPSE,

        _LEN,
    };

    Math::Vec2f size;
    Math::Vec2f position;
    ColorSpace colorSpace = ColorSpace::SRGB;
    Vec<LinearGradient::Stop> stops;

    void repr(Io::Emit& e) const {
        e("(radial-gradient {} {} {} {}", size, position, colorSpace, stops);
    }
};

// MARK: Conic Gradient --------------------------------------------------------
// https://www.w3.org/TR/css-images-4/#conic-gradients

struct ConicGradient {
    struct Stop {
        Gfx::Color color;
        Angle angle;

        void repr(Io::Emit& e) const {
            e("({} {})", color, angle);
        }
    };

    Angle angle;
    Math::Vec2f position;
    ColorSpace colorSpace = ColorSpace::SRGB;
    Vec<LinearGradient::Stop> stops;

    void repr(Io::Emit& e) const {
        e("(conic-gradient {} {} {} {}", angle, position, colorSpace, stops);
    }
};

// MARK: Cross Fade ------------------------------------------------------------
// https://www.w3.org/TR/css-images-4/#cross-fade-function

struct CrossFade {
    struct Layer {
        Box<Image> image;
        Percent opacity;

        void repr(Io::Emit& e) const {
            e("({} {})", image, opacity);
        }
    };

    Vec<Layer> layers;

    void repr(Io::Emit& e) const {
        e("(cross-fade {}", layers);
    }
};

// MARK: Stripes ---------------------------------------------------------------
// https://www.w3.org/TR/css-images-4/#stripes

struct Stripes {
    struct Strip {
        Gfx::Color color;
        Percent size;

        void repr(Io::Emit& e) const {
            e("({} {})", color, size);
        }
    };

    Vec<Strip> stripes;

    void repr(Io::Emit& e) const {
        e("(stripes {}", stripes);
    }
};

// MARK: Image -----------------------------------------------------------------
// https://www.w3.org/TR/css-images-4/#typedef-image

using _Image = Union<
    Gfx::Color,
    LinearGradient,
    RadialGradient,
    ConicGradient,
    CrossFade,
    Stripes,
    Mime::Url>;

struct Image : _Image {
    using _Image::_Image;

    void repr(Io::Emit& e) const {
        visit([&](auto const& i) {
            e("{}", i);
        });
    }
};

} // namespace Vaev
