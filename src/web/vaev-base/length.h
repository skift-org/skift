#pragma once

#include <karm-base/distinct.h>
#include <karm-io/emit.h>
#include <karm-math/fixed.h>
#include <karm-math/rect.h>
#include <karm-math/spacing.h>

#include "base.h"
#include "writing.h"

namespace Vaev {

/// Represents a physical pixel on the actual device screen.
using PhysicalPixel = Distinct<i32, struct _PhysicalPixel>;

/// Represents a logical pixel in the CSS coordinate system.
using Px = Math::i24f8;

using RectPx = Math::Rect<Px>;

using Vec2Px = Math::Vec2<Px>;

using SpacingPx = Math::Spacing<Px>;

// 6. MARK: Distance Units: the <length> type
// https://drafts.csswg.org/css-values/#lengths

struct Length {
    enum struct Unit : u16 {
#define LENGTH(NAME, ...) NAME,
#include "defs/lengths.inc"
#undef LENGTH

        _LEN,
    };

    using enum Unit;

    f64 _val = 0;
    Unit _unit = Unit::PX;

    constexpr f64 val() const {
        return _val;
    }

    constexpr Unit unit() const {
        return _unit;
    }

    constexpr Length() = default;

    constexpr Length(f64 val, Unit unit)
        : _val(val), _unit(unit) {}

    constexpr Length(Px val)
        : _val(val) {}

    constexpr bool isAbsolute() const {
        switch (_unit) {
        case Unit::CM:
        case Unit::MM:
        case Unit::Q:
        case Unit::IN:
        case Unit::PT:
        case Unit::PC:
        case Unit::PX:
            return true;
        default:
            return false;
        }
    }

    constexpr bool isFontRelative() const {
        switch (_unit) {
        case Unit::EM:
        case Unit::REM:
        case Unit::EX:
        case Unit::REX:
        case Unit::CAP:
        case Unit::RCAP:
        case Unit::CH:
        case Unit::RCH:
        case Unit::IC:
        case Unit::RIC:
        case Unit::LH:
        case Unit::RLH:
            return true;

        default:
            return false;
        }
    }

    constexpr bool isViewportRelative() const {
        switch (_unit) {
        case Unit::VW:
        case Unit::SVW:
        case Unit::LVW:
        case Unit::DVW:
        case Unit::VH:
        case Unit::SVH:
        case Unit::LVH:
        case Unit::DVH:
        case Unit::VI:
        case Unit::SVI:
        case Unit::LVI:
        case Unit::DVI:
        case Unit::VB:
        case Unit::SVB:
        case Unit::LVB:
        case Unit::DVB:
        case Unit::VMIN:
        case Unit::SVMIN:
        case Unit::LVMIN:
        case Unit::DVMIN:
        case Unit::VMAX:
        case Unit::SVMAX:
        case Unit::LVMAX:
        case Unit::DVMAX:
            return true;
        default:
            return false;
        }
    }

    constexpr bool isRelative() const {
        return not isAbsolute();
    }

    constexpr bool operator==(Length const &other) const {
        return _val == other._val and _unit == other._unit;
    }

    constexpr std::partial_ordering operator<=>(Length const &other) const {
        if (_unit != other._unit)
            return std::partial_ordering::unordered;
        return _val <=> other._val;
    }

    void repr(Io::Emit &e) const {
        e("{}{}", _val, _unit);
    }
};

struct FontMetrics {
    Px fontSize;
    Px xHeight; //< height of the lowercase 'x'.
    Px capHeight;
    Px zeroAdvance; //< width of the '0' character.
    Px lineHeight;
};

template <>
struct ValueContext<Length> {
    using Resolved = Px;

    Px dpi = Px{96};

    // https://drafts.csswg.org/css-values/#small-viewport-size
    Math::Rect<Px> smallViewport;

    // https://drafts.csswg.org/css-values/#large-viewport-size
    Math::Rect<Px> largeViewport;

    // https://drafts.csswg.org/css-values/#dynamic-viewport-size
    Math::Rect<Px> dynamicViewport;

    InlineAxis inlineAxis;
    BlockAxis blockAxis;

    FontMetrics fontMetrics;
    FontMetrics rootFontMetrics;

    constexpr Px resolve(Length const &l) const {
        switch (l.unit()) {
            // Font-relative

        case Length::Unit::EM:
            return Px::fromFloatNearest(l.val() * fontMetrics.fontSize.toFloat<f64>());

        case Length::Unit::REM:
            return Px::fromFloatNearest(l.val() * rootFontMetrics.fontSize.toFloat<f64>());

        case Length::Unit::EX:
            return Px::fromFloatNearest(l.val() * fontMetrics.xHeight.toFloat<f64>());

        case Length::Unit::REX:
            return Px::fromFloatNearest(l.val() * rootFontMetrics.xHeight.toFloat<f64>());

        case Length::Unit::CAP:
            return Px::fromFloatNearest(l.val() * fontMetrics.capHeight.toFloat<f64>());

        case Length::Unit::RCAP:
            return Px::fromFloatNearest(l.val() * rootFontMetrics.capHeight.toFloat<f64>());

        case Length::Unit::CH:
            return Px::fromFloatNearest(l.val() * fontMetrics.zeroAdvance.toFloat<f64>());

        case Length::Unit::RCH:
            return Px::fromFloatNearest(l.val() * rootFontMetrics.zeroAdvance.toFloat<f64>());

        case Length::Unit::IC:
            return Px::fromFloatNearest(l.val() * fontMetrics.zeroAdvance.toFloat<f64>());

        case Length::Unit::RIC:
            return Px::fromFloatNearest(l.val() * rootFontMetrics.zeroAdvance.toFloat<f64>());

        case Length::Unit::LH:
            return Px::fromFloatNearest(l.val() * fontMetrics.lineHeight.toFloat<f64>());

        case Length::Unit::RLH:
            return Px::fromFloatNearest(l.val() * rootFontMetrics.lineHeight.toFloat<f64>());

        // Viewport-relative

        // https://drafts.csswg.org/css-values/#vw

        // Equal to 1% of the width of current viewport.
        case Length::Unit::VW:
        case Length::Unit::LVW:
            return Px::fromFloatNearest(l.val() * largeViewport.width.toFloat<f64>() / 100);

        case Length::Unit::SVW:
            return Px::fromFloatNearest(l.val() * smallViewport.width.toFloat<f64>() / 100);

        case Length::Unit::DVW:
            return Px::fromFloatNearest(l.val() * dynamicViewport.width.toFloat<f64>() / 100);

        // https://drafts.csswg.org/css-values/#vh
        // Equal to 1% of the height of current viewport.
        case Length::Unit::VH:
        case Length::Unit::LVH:
            return Px::fromFloatNearest(l.val() * largeViewport.height.toFloat<f64>() / 100);

        case Length::Unit::SVH:
            return Px::fromFloatNearest(l.val() * smallViewport.height.toFloat<f64>() / 100);

        case Length::Unit::DVH:
            return Px::fromFloatNearest(l.val() * dynamicViewport.height.toFloat<f64>() / 100);

        // https://drafts.csswg.org/css-values/#vi
        // Equal to 1% of the size of the viewport in the box’s inline axis.
        case Length::Unit::VI:
        case Length::Unit::LVI:
            if (inlineAxis == InlineAxis::HORIZONTAL) {
                return Px::fromFloatNearest(l.val() * largeViewport.width.toFloat<f64>() / 100);
            } else {
                return Px::fromFloatNearest(l.val() * largeViewport.height.toFloat<f64>() / 100);
            }

        case Length::Unit::SVI:
            if (inlineAxis == InlineAxis::HORIZONTAL) {
                return Px::fromFloatNearest(l.val() * smallViewport.width.toFloat<f64>() / 100);
            } else {
                return Px::fromFloatNearest(l.val() * smallViewport.height.toFloat<f64>() / 100);
            }

        case Length::Unit::DVI:
            if (inlineAxis == InlineAxis::HORIZONTAL) {
                return Px::fromFloatNearest(l.val() * dynamicViewport.width.toFloat<f64>() / 100);
            } else {
                return Px::fromFloatNearest(l.val() * dynamicViewport.height.toFloat<f64>() / 100);
            }

        // https://drafts.csswg.org/css-values/#vb
        // Equal to 1% of the size of the viewport in the box’s block axis.
        case Length::Unit::VB:
        case Length::Unit::LVB:
            if (blockAxis == BlockAxis::HORIZONTAL) {
                return Px::fromFloatNearest(l.val() * largeViewport.width.toFloat<f64>() / 100);
            } else {
                return Px::fromFloatNearest(l.val() * largeViewport.height.toFloat<f64>() / 100);
            }

        case Length::Unit::SVB:
            if (blockAxis == BlockAxis::HORIZONTAL) {
                return Px::fromFloatNearest(l.val() * smallViewport.width.toFloat<f64>() / 100);
            } else {
                return Px::fromFloatNearest(l.val() * smallViewport.height.toFloat<f64>() / 100);
            }

        case Length::Unit::DVB:
            if (blockAxis == BlockAxis::HORIZONTAL) {
                return Px::fromFloatNearest(l.val() * dynamicViewport.width.toFloat<f64>() / 100);
            } else {
                return Px::fromFloatNearest(l.val() * dynamicViewport.height.toFloat<f64>() / 100);
            }

        // https://drafts.csswg.org/css-values/#vmin
        // Equal to the smaller of vw and vh.
        case Length::Unit::VMIN:
        case Length::Unit::LVMIN:
            return min(
                resolve(Length(l.val(), Length::Unit::VW)),
                resolve(Length(l.val(), Length::Unit::VH))
            );

        case Length::Unit::SVMIN:
            return min(
                resolve(Length(l.val(), Length::Unit::SVW)),
                resolve(Length(l.val(), Length::Unit::SVH))
            );

        case Length::Unit::DVMIN:
            return min(
                resolve(Length(l.val(), Length::Unit::DVW)),
                resolve(Length(l.val(), Length::Unit::DVH))
            );

        // https://drafts.csswg.org/css-values/#vmax
        // Equal to the larger of vw and vh.
        case Length::Unit::VMAX:
        case Length::Unit::LVMAX:
            return max(
                resolve(Length(l.val(), Length::Unit::VW)),
                resolve(Length(l.val(), Length::Unit::VH))
            );

        case Length::Unit::DVMAX:
            return max(
                resolve(Length(l.val(), Length::Unit::DVW)),
                resolve(Length(l.val(), Length::Unit::DVH))
            );

        case Length::Unit::SVMAX:
            return max(
                resolve(Length(l.val(), Length::Unit::SVW)),
                resolve(Length(l.val(), Length::Unit::SVH))
            );

        // Absolute
        // https://drafts.csswg.org/css-values/#absolute-lengths
        case Length::Unit::CM:
            return Px::fromFloatNearest(l.val() * dpi.toFloat<f64>() / 2.54);

        case Length::Unit::MM:
            return Px::fromFloatNearest(l.val() * dpi.toFloat<f64>() / 25.4);

        case Length::Unit::Q:
            return Px::fromFloatNearest(l.val() * dpi.toFloat<f64>() / 101.6);

        case Length::Unit::IN:
            return Px::fromFloatNearest(l.val() * dpi.toFloat<f64>());

        case Length::Unit::PT:
            return Px::fromFloatNearest(l.val() * dpi.toFloat<f64>() / 72.0);

        case Length::Unit::PC:
            return Px::fromFloatNearest(l.val() * dpi.toFloat<f64>() / 6.0);

        case Length::Unit::PX:
            return Px::fromFloatNearest(l.val());

        default:
            panic("invalid length unit");
        }
    }
};

} // namespace Vaev
