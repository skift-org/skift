#pragma once

#include <karm-io/emit.h>
#include <karm-math/const.h>

namespace Vaev {

// 7.1. MARK: Angle Units: the <angle> type and deg, grad, rad, turn units
// https://drafts.csswg.org/css-values/#angles

struct Angle {
    enum struct Unit {
        DEGREE,
        RADIAN,
        GRAD,
        TURN,
    };

    f64 _val;
    Unit _unit;

    static constexpr Angle fromDegree(f64 val) {
        return {val, Unit::DEGREE};
    }

    static constexpr Angle fromRadian(f64 val) {
        return {val, Unit::RADIAN};
    }

    static constexpr Angle fromGrad(f64 val) {
        return {val, Unit::GRAD};
    }

    static constexpr Angle fromTurn(f64 val) {
        return {val, Unit::TURN};
    }

    constexpr f64 val() const {
        return _val;
    }

    constexpr Unit unit() const {
        return _unit;
    }

    constexpr f64 toDegree() const {
        switch (_unit) {
        case Unit::DEGREE:
            return _val;
        case Unit::RADIAN:
            return _val * 180.0 / Math::PI;
        case Unit::GRAD:
            return _val * 0.9;
        case Unit::TURN:
            return _val * 360.0;
        }
    }

    constexpr f64 toRadian() const {
        switch (_unit) {
        case Unit::DEGREE:
            return _val * Math::PI / 180.0;
        case Unit::RADIAN:
            return _val;
        case Unit::GRAD:
            return _val * Math::PI / 200.0;
        case Unit::TURN:
            return _val * 2.0 * Math::PI;
        }
    }

    constexpr bool operator==(Angle const &other) const {
        return toRadian() == other.toRadian();
    }

    constexpr std::partial_ordering operator<=>(Angle const &other) const {
        return toRadian() <=> other.toRadian();
    }

    constexpr Angle operator+(Angle other) const {
        return fromRadian(toRadian() + other.toRadian());
    }

    constexpr Angle operator-(Angle other) const {
        return fromRadian(toRadian() - other.toRadian());
    }

    void repr(Io::Emit &e) const {
        switch (_unit) {
        case Unit::DEGREE:
            e("{}deg", _val);
            break;
        case Unit::RADIAN:
            e("{}rad", _val);
            break;
        case Unit::GRAD:
            e("{}grad", _val);
            break;
        case Unit::TURN:
            e("{}turn", _val);
            break;
        }
    }
};

} // namespace Vaev
