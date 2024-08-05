#pragma once

#include "vec.h"

namespace Karm::Math {

template <typename T>
struct Insets {
    T start;
    T top;
    T end;
    T bottom;

    constexpr Insets()
        : start(), top(), end(), bottom() {}

    constexpr Insets(T all)
        : start(all), top(all), end(all), bottom(all) {}

    constexpr Insets(T horizontal, T vertical)
        : start(horizontal), top(vertical), end(horizontal), bottom(vertical) {}

    constexpr Insets(T top, T horizontal, T bottom)
        : start(horizontal), top(top), end(horizontal), bottom(bottom) {}

    constexpr Insets(T start, T top, T end, T bottom)
        : start(start), top(top), end(end), bottom(bottom) {}

    constexpr bool zero() const {
        return start == 0 and top == 0 and end == 0 and bottom == 0;
    }

    constexpr Math::Vec2<T> topStart() const {
        return {start, top};
    }

    constexpr Math::Vec2<T> topEnd() const {
        return {end, top};
    }

    constexpr Math::Vec2<T> bottomStart() const {
        return {start, bottom};
    }

    constexpr Math::Vec2<T> bottomEnd() const {
        return {end, bottom};
    }

    constexpr Math::Vec2<T> all() const {
        return {start + end, top + bottom};
    }

    void repr(Io::Emit &e) const {
        e("(insets {} {} {} {})", start, top, end, bottom);
    }
};

using Insetsi = Insets<isize>;
using Insetsf = Insets<f64>;

} // namespace Karm::Math
