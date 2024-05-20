#pragma once

#include <karm-math/const.h>

namespace Web::Unit {

// 7.4. MARK: Resolution Units: the <resolution> type and dpi, dpcm, dppx units
// https://drafts.csswg.org/css-values/#resolution

struct Resolution {
    enum struct Unit {
        DPI,
        DPCM,
        DPPX,
        INFINITE,
    };

    using enum Unit;

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
            return Math::INF;
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
            return Math::INF;
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
            return Math::INF;
        }
    }

    bool operator==(Resolution const &other) const {
        return _val == other._val and _unit == other._unit;
    }

    std::partial_ordering operator<=>(Resolution const &other) const {
        return toDpi() <=> other.toDpi();
    }
};

} // namespace Web::Unit
