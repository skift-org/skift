#pragma once

#include <karm-gfx/fill.h>
#include <karm-mime/url.h>

#include "calc.h"
#include "color.h"
#include "image.h"
#include "keywords.h"
#include "length.h"
#include "percent.h"

namespace Vaev {

enum struct BackgroundAttachment {
    SCROLL,
    FIXED,
    LOCAL,

    _LEN
};

struct BackgroundPosition {
    using HorizontalAnchor = Union<Keywords::Left, Keywords::Center, Keywords::Right>;
    using VerticalAnchor = Union<Keywords::Top, Keywords::Center, Keywords::Bottom>;

    HorizontalAnchor horizontalAnchor;
    CalcValue<PercentOr<Length>> horizontal;
    VerticalAnchor verticalAnchor;
    CalcValue<PercentOr<Length>> vertical;

    constexpr BackgroundPosition()
        : horizontalAnchor(Keywords::LEFT),
          horizontal(Percent{0}),
          verticalAnchor(Keywords::TOP),
          vertical(Percent{0}) {
    }

    constexpr BackgroundPosition(CalcValue<PercentOr<Length>> horizontal, CalcValue<PercentOr<Length>> vertical)
        : horizontalAnchor(Keywords::LEFT), horizontal(horizontal), verticalAnchor(Keywords::TOP), vertical(vertical) {
    }

    constexpr BackgroundPosition(HorizontalAnchor horizontalAnchor, CalcValue<PercentOr<Length>> horizontal, VerticalAnchor verticalAnchor, CalcValue<PercentOr<Length>> vertical)
        : horizontalAnchor(horizontalAnchor), horizontal(horizontal), verticalAnchor(verticalAnchor), vertical(vertical) {
    }

    void repr(Io::Emit& e) const {
        e("({} {}", horizontalAnchor, horizontal);

        e(" ");
        e("{} {})", verticalAnchor, vertical);
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

    constexpr BackgroundRepeat(_Val val = REPEAT)
        : _val(static_cast<u8>(val)) {
    }

    void repr(Io::Emit& e) const {
        if (_val == NO) {
            e("no-repeat");
        } else if (_val == REPEAT) {
            e("repeat");
        } else {
            if (_val & X)
                e("repeat-x");
            if (_val & Y)
                e("repeat-y");
        }
    }
};

struct BackgroundLayer {
    Opt<Image> image;
    BackgroundAttachment attachment;
    BackgroundPosition position;
    BackgroundRepeat repeat;

    void repr(Io::Emit& e) const {
        e("(background");
        e(" image={}", image);
        e(" attachment={}", attachment);
        e(" position={}", position);
        e(" repeat={}", repeat);
        e(")");
    }
};

struct BackgroundProps {
    Color color = TRANSPARENT;
    Vec<BackgroundLayer> layers = {};

    void repr(Io::Emit& e) const {
        e("(background");
        e(" color={}", color);
        e(" layers={}", layers);
        e(")");
    }
};

} // namespace Vaev
