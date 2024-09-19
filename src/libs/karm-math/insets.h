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

    constexpr Insets(T vertical, T horizontal)
        : start(horizontal), top(vertical), end(horizontal), bottom(vertical) {}

    constexpr Insets(T top, T horizontal, T bottom)
        : start(horizontal), top(top), end(horizontal), bottom(bottom) {}

    constexpr Insets(T top, T end, T bottom, T start)
        : start(start), top(top), end(end), bottom(bottom) {}

    constexpr bool zero() const {
        return start == T{} and top == T{} and end == T{} and bottom == T{};
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

    constexpr T horizontal() const {
        return start + end;
    }

    constexpr T vertical() const {
        return top + bottom;
    }

    void repr(Io::Emit &e) const {
        e("(insets {} {} {} {})", start, top, end, bottom);
    }

    constexpr auto map(auto f) const {
        using U = decltype(f(start));
        return Insets<U>{f(start), f(top), f(end), f(bottom)};
    }
};

using Insetsi = Insets<isize>;
using Insetsf = Insets<f64>;

} // namespace Karm::Math
