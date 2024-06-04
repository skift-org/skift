#pragma once

#include <karm-gfx/paint.h>
#include <karm-mime/url.h>

#include "length.h"
#include "percent.h"

namespace Web::Types {

enum struct BackgroundAttachment {
    SCROLL,
    FIXED,
    LOCAL,
};

struct BackgroundPosition {
    enum struct HorizontalAnchor {
        LEFT,
        CENTER,
        RIGHT,
    };

    enum struct VerticalPosition {
        TOP,
        CENTER,
        BOTTOM,
    };

    HorizontalAnchor horizontalAnchor;
    PercentOr<Length> horizontal;
    VerticalPosition verticalAnchor;
    PercentOr<Length> vertical;

    BackgroundPosition(PercentOr<Length> horizontal, PercentOr<Length> vertical)
        : horizontalAnchor(HorizontalAnchor::LEFT), horizontal(horizontal), verticalAnchor(VerticalPosition::TOP), vertical(vertical) {
    }

    BackgroundPosition(HorizontalAnchor horizontalAnchor, PercentOr<Length> horizontal, VerticalPosition verticalAnchor, PercentOr<Length> vertical)
        : horizontalAnchor(horizontalAnchor), horizontal(horizontal), verticalAnchor(verticalAnchor), vertical(vertical) {
    }
};

struct BackgroundRepeat {
    enum _Val : u8 {
        NO = 0,
        X = 1 << 0,
        Y = 1 << 1,
        REPEAT = X | Y,
    };

    u8 _val;

    BackgroundRepeat(_Val val) : _val(static_cast<u8>(val)) {
    }
};

struct Background {
    Gfx::Paint paint;
    Opt<Mime::Url> image;
    BackgroundAttachment attachment;
    BackgroundPosition position;
    BackgroundRepeat repeat;
};

} // namespace Web::Types
