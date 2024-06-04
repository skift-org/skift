#pragma once

#include <karm-math/const.h>

namespace Web::Types {

// 7.2. MARK: Duration Units: the <time> type and s, ms units
// https://drafts.csswg.org/css-values/#time

struct Time {
    enum struct Unit {
        S,
        MS
    };

    using enum Unit;

    f64 _val;
    Unit _unit;

    static Time fromSecond(f64 val) {
        return {val, Unit::S};
    }

    static Time fromMillisecond(f64 val) {
        return {val, Unit::MS};
    }

    f64 val() const {
        return _val;
    }

    Unit unit() const {
        return _unit;
    }

    f64 toSecond() const {
        switch (_unit) {
        case Unit::S:
            return _val;
        case Unit::MS:
            return _val / 1000.0;
        }
    }

    f64 toMillisecond() const {
        switch (_unit) {
        case Unit::S:
            return _val * 1000.0;
        case Unit::MS:
            return _val;
        }
    }

    bool operator==(Time const &other) const {
        return _val == other._val && _unit == other._unit;
    }

    auto operator<=>(Time const &other) const {
        return toSecond() <=> other.toSecond();
    }
};

} // namespace Web::Types
