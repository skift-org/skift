#pragma once

#include <karm-base/array.h>
#include <karm-base/iter.h>

#include "insets.h"
#include "vec.h"

namespace Karm::Math {

template <typename T>
struct Radii {
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

    constexpr Radii(T all = {})
        : a(all), b(all), c(all), d(all), e(all), f(all), g(all), h(all) {}

    constexpr Radii(T StartEnd, T EndStart)
        : Radii(StartEnd, EndStart, StartEnd, EndStart) {}

    constexpr Radii(T topStart, T topEnd, T bottomEnd, T bottomStart)
        : a(topStart), b(topStart),
          c(topEnd), d(topEnd),
          e(bottomEnd), f(bottomEnd),
          g(bottomStart), h(bottomStart) {}

    constexpr Radii(T a, T b, T c, T d, T e, T f, T g, T h)
        : a(a), b(b), c(c), d(d), e(e), f(f), g(g), h(h) {}

    bool zero() const {
        return iter(radii).all([](T radii) {
            return epsilonEq(radii, T{});
        });
    }

    T all() const {
        return a;
    }

    void all(T all) {
        for (auto &radii : radii) {
            radii = all;
        }
    }

    Radii reduceOverlap(Vec2<T> size) const {
        auto res = *this;
        auto scaleAll = [&](T factor) {
            if (factor >= 1)
                return;
            for (auto &radii : res.radii)
                radii *= factor;
        };

        for (auto &r : res.radii)
            r = max(r, T{});

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

    Radii shrink(Insets<T> insets) const {
        auto res = *this;

        res.a = max(res.a - insets.top, T{});
        res.b = max(res.b - insets.start, T{});
        res.c = max(res.c - insets.end, T{});
        res.d = max(res.d - insets.top, T{});

        res.e = max(res.e - insets.bottom, T{});
        res.f = max(res.f - insets.end, T{});
        res.g = max(res.g - insets.start, T{});
        res.h = max(res.h - insets.bottom, T{});

        return res;
    }

    Radii grow(Insets<T> insets) const {
        auto res = *this;

        res.a += insets.top;
        res.b += insets.start;
        res.c += insets.end;
        res.d += insets.top;

        res.e += insets.bottom;
        res.f += insets.end;
        res.g += insets.start;
        res.h += insets.bottom;

        return res;
    }

    template <typename U>
    constexpr Radii<U> cast() const {
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

    void repr(Io::Emit &_e) const {
        _e("(radii {} {} {} {} {} {} {} {})", a, b, c, d, e, f, g, h);
    }

    constexpr auto map(auto f) const {
        using U = decltype(f(a));
        return Radii<U>{
            f(a),
            f(b),
            f(c),
            f(d),
            f(e),
            f(f),
            f(g),
            f(h),
        };
    }
};

using Radiii = Radii<isize>;

using Radiif = Radii<f64>;

} // namespace Karm::Math
