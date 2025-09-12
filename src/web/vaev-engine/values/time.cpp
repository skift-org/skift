export module Vaev.Engine:values.time;

import Karm.Core;

using namespace Karm;

namespace Vaev {

// 7.2. MARK: Duration Units: the <time> type and s, ms units
// https://drafts.csswg.org/css-values/#time

export struct Time {
    enum struct Unit {
        S,
        MS
    };

    using enum Unit;

    f64 _val;
    Unit _unit;

    static constexpr Time fromSecond(f64 val) {
        return {val, Unit::S};
    }

    static constexpr Time fromMillisecond(f64 val) {
        return {val, Unit::MS};
    }

    constexpr f64 val() const {
        return _val;
    }

    constexpr Unit unit() const {
        return _unit;
    }

    constexpr f64 toSecond() const {
        switch (_unit) {
        case Unit::S:
            return _val;
        case Unit::MS:
            return _val / 1000.0;
        }
    }

    constexpr f64 toMillisecond() const {
        switch (_unit) {
        case Unit::S:
            return _val * 1000.0;
        case Unit::MS:
            return _val;
        }
    }

    constexpr bool operator==(Time const& other) const {
        return _val == other._val and _unit == other._unit;
    }

    constexpr auto operator<=>(Time const& other) const {
        return toSecond() <=> other.toSecond();
    }
};

} // namespace Vaev
