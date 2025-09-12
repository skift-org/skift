module;

#include <karm-core/macros.h>

export module Vaev.Engine:values.resolution;

import Karm.Core;

import :css;
import :values.base;

using namespace Karm;

namespace Vaev {

// 7.4. MARK: Resolution Units: the <resolution> type and dpi, dpcm, dppx units
// https://drafts.csswg.org/css-values/#resolution

export struct Resolution {
    enum struct Unit {
        DPI,
        DPCM,
        DPPX,
        X = DPPX,
        INFINITE,
    };

    using enum Unit;

    f64 _val;
    Unit _unit;

    static Resolution fromDpi(f64 val) {
        return {val, Unit::DPI};
    }

    static Resolution fromDpcm(f64 val) {
        return {val, Unit::DPCM};
    }

    static Resolution fromDppx(f64 val) {
        return {val, Unit::DPPX};
    }

    static Resolution infinite() {
        return {0.0, Unit::INFINITE};
    }

    constexpr f64 val() const {
        return _val;
    }

    constexpr Unit unit() const {
        return _unit;
    }

    constexpr f64 toDpi() const {
        switch (_unit) {
        case Unit::DPI:
            return _val;
        case Unit::DPCM:
            return _val * 2.54;
        case Unit::DPPX:
            return _val * 96.0;
        case Unit::INFINITE:
            return Math::INF;
        }
    }

    constexpr f64 toDpcm() const {
        switch (_unit) {
        case Unit::DPI:
            return _val / 2.54;
        case Unit::DPCM:
            return _val;
        case Unit::DPPX:
            return _val * 96.0 / 2.54;
        case Unit::INFINITE:
            return Math::INF;
        }
    }

    constexpr f64 toDppx() const {
        switch (_unit) {
        case Unit::DPI:
            return _val / 96.0;
        case Unit::DPCM:
            return _val * 2.54 / 96.0;
        case Unit::DPPX:
            return _val;
        case Unit::INFINITE:
            return Math::INF;
        }
    }

    constexpr bool operator==(Resolution const& other) const {
        return _val == other._val and _unit == other._unit;
    }

    std::partial_ordering operator<=>(Resolution const& other) const {
        return toDpi() <=> other.toDpi();
    }

    constexpr void repr(Io::Emit& e) const {
        switch (_unit) {
        case Unit::DPI:
            e("{}dpi", _val);
            break;
        case Unit::DPCM:
            e("{}dpcm", _val);
            break;
        case Unit::DPPX:
            e("{}dppx", _val);
            break;
        case Unit::INFINITE:
            e("infinite");
            break;
        }
    }
};

export template <>
struct ValueParser<Resolution> {
    static Res<Resolution::Unit> _parseResolutionUnit(Str unit) {
        if (eqCi(unit, "dpi"s))
            return Ok(Resolution::Unit::DPI);
        else if (eqCi(unit, "dpcm"s))
            return Ok(Resolution::Unit::DPCM);
        else if (eqCi(unit, "dppx"s) or eqCi(unit, "x"s))
            return Ok(Resolution::Unit::DPPX);
        else
            return Error::invalidData("unknown resolution unit");
    }

    static Res<Resolution> parse(Cursor<Css::Sst>& c) {
        if (c.ended())
            return Error::invalidData("unexpected end of input");

        if (c.peek() == Css::Token::DIMENSION) {
            Io::SScan scan = c->token.data.str();
            auto value = Io::atof(scan).unwrapOr(0.0);
            auto unit = try$(_parseResolutionUnit(scan.remStr()));
            return Ok(Resolution{value, unit});
        }

        return Error::invalidData("expected resolution");
    }
};

} // namespace Vaev
