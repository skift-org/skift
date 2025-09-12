module;

#include <karm-math/au.h>

export module Vaev.Engine:values.length;

import Karm.Core;

import :css;
import :values.base;
import :values.resolved;

using namespace Karm;

namespace Vaev {

// 6. MARK: Distance Units: the <length> type
// https://drafts.csswg.org/css-values/#lengths

export struct [[gnu::packed]] Length {
    enum struct Unit : u8 {
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

    constexpr Length(Au val)
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

    constexpr bool operator==(Length const& other) const {
        return _val == other._val and _unit == other._unit;
    }

    constexpr std::partial_ordering operator<=>(Length const& other) const {
        if (_unit != other._unit)
            return std::partial_ordering::unordered;
        return _val <=> other._val;
    }

    void repr(Io::Emit& e) const {
        e("{}{}", _val, _unit);
    }
};

export template <>
struct _Resolved<Length> {
    using Type = Au;
};

export template <>
struct ValueParser<Length> {
    static Res<Length::Unit> _parseLengthUnit(Str unit) {
#define LENGTH(NAME, ...)      \
    if (eqCi(unit, #NAME ""s)) \
        return Ok(Length::Unit::NAME);
#include "defs/lengths.inc"

#undef LENGTH

        return Error::invalidData("unknown length unit");
    }

    static Res<Length> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::DIMENSION) {
            Io::SScan scan = c->token.data.str();
            auto value = Io::atof(scan, {.allowExp = false}).unwrapOr(0.0);
            auto unit = try$(_parseLengthUnit(scan.remStr()));
            c.next();

            return Ok(Length{value, unit});
        }

        if (c.skip(Css::Token::number("0"))) {
            return Ok(Length{0.0, Length::Unit::PX});
        }

        return Error::invalidData("expected length");
    }
};

} // namespace Vaev
