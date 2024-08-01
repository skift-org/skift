#pragma once

#include <karm-base/array.h>
#include <karm-base/iter.h>

#include "vec.h"

namespace Karm::Math {

template <typename T>
struct Radius {
    /*
        The radii are disposed in the following order

              b        c
              . │    │
             ┌──┤....├──┐
           a │  │    │  │ d
             │          │
            ─┴─        ─┴─
             :          :
             :          :
            ─┬─        ─┬─
             │          │
           h │  │    │  │ e
             └──┤....├──┘
                │    │
              g        f

        border-radius: b c f g / a d e h
    */

    union {
        struct {
            T a, b, c, d, e, f, g, h;
        };
        Array<T, 8> radii;
    };

    constexpr Radius(T all = {})
        : a(all), b(all), c(all), d(all), e(all), f(all), g(all), h(all) {}

    constexpr Radius(T StartEnd, T EndStart)
        : Radius(StartEnd, EndStart, StartEnd, EndStart) {}

    constexpr Radius(T topStart, T topEnd, T bottomEnd, T bottomStart)
        : a(topStart), b(topStart),
          c(topEnd), d(topEnd),
          e(bottomEnd), f(bottomEnd),
          g(bottomStart), h(bottomStart) {}

    constexpr Radius(T a, T b, T c, T d, T e, T f, T g, T h)
        : a(a), b(b), c(c), d(d), e(e), f(f), g(g), h(h) {}

    bool zero() const {
        return iter(radii).all([](T radius) {
            return radius == 0;
        });
    }

    T all() const {
        return a;
    }

    void all(T all) {
        for (auto &radius : radii) {
            radius = all;
        }
    }

    Radius reduceOverlap(Vec2<T> size) const {
        auto res = *this;
        auto scaleAll = [&](T factor) {
            if (factor >= 1)
                return;
            for (auto &radius : res.radii)
                radius *= factor;
        };

        auto sumTop = res.b + res.c;
        scaleAll(sumTop > T{} ? size.width / sumTop : 1);

        auto sumEnd = res.d + res.e;
        scaleAll(sumEnd > T{} ? size.height / sumEnd : 1);

        auto sumBottom = res.f + res.g;
        scaleAll(sumBottom > T{} ? size.width / sumBottom : 1);

        auto sumStart = res.h + res.a;
        scaleAll(sumStart > T{} ? size.height / sumStart : 1);
        return res;
    }

    template <typename U>
    constexpr Radius<U> cast() const {
        return {
            static_cast<U>(a),
            static_cast<U>(b),
            static_cast<U>(c),
            static_cast<U>(d),
            static_cast<U>(e),
            static_cast<U>(f),
            static_cast<U>(g),
            static_cast<U>(h),
        };
    }
};

using Radiusi = Radius<isize>;

using Radiusf = Radius<f64>;

} // namespace Karm::Math

template <typename T>
ReflectableTemplate$(Karm::Math::Radius<T>, a, b, c, d, e, f, g, h);
