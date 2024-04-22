#pragma once

#include <karm-base/distinct.h>
#include <karm-math/fixed.h>
#include <karm-math/rect.h>

namespace Web::Css {

/// Represents a physical pixel on the actual device screen.
using PhysicalPixel = Distinct<i32, struct _PhysicalPixel>;

/// Represents a logical pixel in the CSS coordinate system.
using Px = Math::i24f8;

// 6. MARK: Distance Units: the <length> type ----------------------------------
// https://drafts.csswg.org/css-values/#lengths

struct Length {
    enum struct Unit : u16 {
        VIEWPORT_NONE = 0b00 << 8,
        VIEWPORT_SMALL = 0b01 << 8,
        VIEWPORT_LARGE = 0b10 << 8,
        VIEWPORT_DYNAMIC = 0b11 << 8,

        AXIS_NONE = 0b000 << 10,
        AXIS_INLINE = 0b001 << 10,
        AXIS_BLOCK = 0b010 << 10,
        AXIS_WIDTH = 0b011 << 10,
        AXIS_HEIGHT = 0b100 << 10,

        // Font-relative
        EM = 0,
        REM,
        EX,
        REX,
        CAP,
        RCAP,
        CH,
        RCH,
        IC,
        RIC,
        LH,
        RLH,

        // Viewport-relative
        VW,
        SVW,
        LVW,
        DVW,

        VH,
        SVH,
        LVH,
        DVH,

        VI,
        SVI,
        LVI,
        DVI,

        VB,
        SVB,
        LVB,
        DVB,

        VMIN,
        SVMIN,
        LVMIN,
        DVMIN,

        VMAX,
        SVMAX,
        LVMAX,
        DVMAX,

        // Absolute

        CM, // centimeters
        MM, // millimeters
        Q,  // quarter-millimeters
        IN, // inches
        PT, // points
        PC, // picas
        PX, // pixels
    };

    f64 _val;
    Unit _unit;

    f64 val() const {
        return _val;
    }

    Unit unit() const {
        return _unit;
    }

    Length() = default;

    Length(f64 val, Unit unit)
        : _val(val), _unit(unit) {}

    Length(Px val)
        : _val(val), _unit(Unit::PX) {}

    bool isAbsolute() const {
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

    bool isFontRelative() const {
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

    bool isViewportRelative() const {
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

    bool isRelative() const {
        return not isAbsolute();
    }

    bool operator==(Length const &other) const {
        return _val == other._val and _unit == other._unit;
    }

    std::partial_ordering operator<=>(Length const &other) const {
        if (_unit != other._unit)
            return std::partial_ordering::unordered;
        return _val <=> other._val;
    }
};

struct FontMetrics {
    Px fontSize;
    Px xHeight;
    Px capHeight;
    Px zeroAdvance;
    Px lineHeight;
};

// https://drafts.csswg.org/css-writing-modes-4/#inline-axis
enum struct InlineAxis {
    HORIZONTAL,
    VERTICAL,
};

// https://drafts.csswg.org/css-writing-modes-4/#block-axis
enum struct BlockAxis {
    HORIZONTAL,
    VERTICAL,
};

struct LengthContext {
    Px dpi;

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

    Px toPx(Length const &l) const {
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
                toPx(Length(l.val(), Length::Unit::VW)),
                toPx(Length(l.val(), Length::Unit::VH))
            );

        case Length::Unit::SVMIN:
            return min(
                toPx(Length(l.val(), Length::Unit::SVW)),
                toPx(Length(l.val(), Length::Unit::SVH))
            );

        case Length::Unit::DVMIN:
            return min(
                toPx(Length(l.val(), Length::Unit::DVW)),
                toPx(Length(l.val(), Length::Unit::DVH))
            );

        // https://drafts.csswg.org/css-values/#vmax
        // Equal to the larger of vw and vh.
        case Length::Unit::VMAX:
        case Length::Unit::LVMAX:
            return max(
                toPx(Length(l.val(), Length::Unit::VW)),
                toPx(Length(l.val(), Length::Unit::VH))
            );

        case Length::Unit::DVMAX:
            return max(
                toPx(Length(l.val(), Length::Unit::DVW)),
                toPx(Length(l.val(), Length::Unit::DVH))
            );

        case Length::Unit::SVMAX:
            return max(
                toPx(Length(l.val(), Length::Unit::SVW)),
                toPx(Length(l.val(), Length::Unit::SVH))
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

// 7.1. MARK: Angle Units: the <angle> type and deg, grad, rad, turn units -----
// https://drafts.csswg.org/css-values/#angles

struct Angle {
    enum struct Unit {
        DEGREE,
        RADIAN,
        GRAD,
        TURN,
    };

    double _val;
    Unit _unit;

    static Angle fromDegree(double val) {
        return {val, Unit::DEGREE};
    }

    static Angle fromRadian(double val) {
        return {val, Unit::RADIAN};
    }

    static Angle fromGrad(double val) {
        return {val, Unit::GRAD};
    }

    static Angle fromTurn(double val) {
        return {val, Unit::TURN};
    }

    double val() const {
        return _val;
    }

    Unit unit() const {
        return _unit;
    }

    double toDegree() const {
        switch (_unit) {
        case Unit::DEGREE:
            return _val;
        case Unit::RADIAN:
            return _val * 180.0 / M_PI;
        case Unit::GRAD:
            return _val * 0.9;
        case Unit::TURN:
            return _val * 360.0;
        }
    }

    double toRadian() const {
        switch (_unit) {
        case Unit::DEGREE:
            return _val * M_PI / 180.0;
        case Unit::RADIAN:
            return _val;
        case Unit::GRAD:
            return _val * M_PI / 200.0;
        case Unit::TURN:
            return _val * 2.0 * M_PI;
        }
    }

    bool operator==(Angle const &other) const {
        return _val == other._val and _unit == other._unit;
    }

    std::partial_ordering operator<=>(Angle const &other) const {
        return toRadian() <=> other.toRadian();
    }
};

// 7.4. MARK: Resolution Units: the <resolution> type and dpi, dpcm, dppx units
// https://drafts.csswg.org/css-values/#resolution

struct Resolution {
    enum struct Unit {
        DPI,
        DPCM,
        DPPX,
        INFINITE,
    };

    double _val;
    Unit _unit;

    static Resolution fromDpi(double val) {
        return {val, Unit::DPI};
    }

    static Resolution fromDpcm(double val) {
        return {val, Unit::DPCM};
    }

    static Resolution fromDppx(double val) {
        return {val, Unit::DPPX};
    }

    static Resolution infinite() {
        return {0.0, Unit::INFINITE};
    }

    double val() const {
        return _val;
    }

    Unit unit() const {
        return _unit;
    }

    double toDpi() const {
        switch (_unit) {
        case Unit::DPI:
            return _val;
        case Unit::DPCM:
            return _val * 2.54;
        case Unit::DPPX:
            return _val * 96.0;
        case Unit::INFINITE:
            return INFINITY;
        }
    }

    double toDpcm() const {
        switch (_unit) {
        case Unit::DPI:
            return _val / 2.54;
        case Unit::DPCM:
            return _val;
        case Unit::DPPX:
            return _val * 96.0 / 2.54;
        case Unit::INFINITE:
            return INFINITY;
        }
    }

    double toDppx() const {
        switch (_unit) {
        case Unit::DPI:
            return _val / 96.0;
        case Unit::DPCM:
            return _val * 2.54 / 96.0;
        case Unit::DPPX:
            return _val;
        case Unit::INFINITE:
            return INFINITY;
        }
    }

    bool operator==(Resolution const &other) const {
        return _val == other._val and _unit == other._unit;
    }

    std::partial_ordering operator<=>(Resolution const &other) const {
        return toDpi() <=> other.toDpi();
    }
};

} // namespace Web::Css
