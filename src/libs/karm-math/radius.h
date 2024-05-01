#pragma once

#include <karm-base/std.h>

namespace Karm::Math {

template <typename T>
struct Radius {
    union {
        T topStart{};
        T all;
    };
    T topEnd{};
    T bottomStart{};
    T bottomEnd{};

    constexpr Radius() = default;

    constexpr Radius(T all)
        : topStart(all),
          topEnd(all),
          bottomStart(all),
          bottomEnd(all) {}

    constexpr Radius(T StartEnd, T EndStart)
        : topStart(StartEnd),
          topEnd(EndStart),
          bottomStart(EndStart),
          bottomEnd(StartEnd) {}

    constexpr Radius(T topStart, T topEnd, T bottomStart, T bottomEnd)
        : topStart(topStart),
          topEnd(topEnd),
          bottomStart(bottomStart),
          bottomEnd(bottomEnd) {}

    bool zero() const {
        return topStart == 0 and topEnd == 0 and
               bottomStart == 0 and bottomEnd == 0;
    }

    Radius clamp(T min, T max) const {
        return {
            clamp(topStart, min, max),
            clamp(topEnd, min, max),
            clamp(bottomStart, min, max),
            clamp(bottomEnd, min, max),
        };
    }
};

using Radiusi = Radius<isize>;

using Radiusf = Radius<f64>;

} // namespace Karm::Math
