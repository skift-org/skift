#pragma once

#include <karm-base/distinct.h>
#include <karm-io/emit.h>
#include <karm-math/fixed.h>
#include <karm-math/insets.h>
#include <karm-math/radii.h>
#include <karm-math/rect.h>

namespace Vaev {

/// Represents a physical pixel on the actual device screen.
using PhysicalPixel = Distinct<i32, struct _PhysicalPixel>;

/// Represents a logical pixel in the CSS coordinate system.
using Px = Math::i24f8;

using RectPx = Math::Rect<Px>;

using Vec2Px = Math::Vec2<Px>;

using InsetsPx = Math::Insets<Px>;

using RadiiPx = Math::Radii<Px>;

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

    using Resolved = Px;

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
        : _val(val.cast<f64>()) {}

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

} // namespace Vaev
