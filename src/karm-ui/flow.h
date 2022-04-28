#pragma once

#include <karm-math/rect.h>

namespace Karm::Ui {

struct Flow {
    enum _F {
        LTR,
        RTL,
        TTB,
        BTT,
    };

    _F _f;

    Flow(_F f) : _f(f) {}

    template <typename T>
    auto vec() -> Math::Vec2<T> {
        switch (_f) {
        case LTR:
            return {1, 0};
        case RTL:
            return {-1, 0};
        case TTB:
            return {0, 1};
        case BTT:
            return {0, -1};
        }
    }

    template <typename T>
    auto start(Math::Rect<T> r) -> T {
        switch (_f) {
        case LTR:
            return r.start();
        case RTL:
            return r.end();
        case TTB:
            return r.top();
        case BTT:
            return r.bottom();
        }
    }

    template <typename T>
    void start(Math::Rect<T> &r, T v) {
        switch (_f) {
        case LTR:
            r.start(v);
            break;
        case RTL:
            r.end(v);
            break;
        case TTB:
            r.top(v);
            break;
        case BTT:
            r.bottom(v);
            break;
        }
    }

    template <typename T>
    auto end(Math::Rect<T> r) -> T {
        switch (_f) {
        case LTR:
            return r.end();
        case RTL:
            return r.x;
        case TTB:
            return r.y + r.height;
        case BTT:
            return r.y;
        }
    }

    template <typename T>
    void end(Math::Rect<T> &r, T v) {
        switch (_f) {
        case LTR:
            r.end(v);
            break;
        case RTL:
            r.start(v);
            break;
        case TTB:
            r.bottom(v);
            break;
        case BTT:
            r.top(v);
            break;
        }
    }

    template <typename T>
    auto top(Math::Rect<T> r) -> T {
        switch (_f) {
        case LTR:
        case RTL:
            return r.y;

        case TTB:
        case BTT:
            return r.x;
        }
    }

    template <typename T>
    void top(Math::Rect<T> &r, T v) {
        switch (_f) {
        case LTR:
        case RTL:
            r.top(v);
            break;

        case TTB:
        case BTT:
            r.start(v);
            break;
        }
    }

    template <typename T>
    auto bottom(Math::Rect<T> r) -> T {
        switch (_f) {
        case LTR:
        case RTL:
            return r.y + r.height;

        case TTB:
        case BTT:
            return r.x + r.height;
        }
    }

    template <typename T>
    void bottom(Math::Rect<T> &r, T v) {
        switch (_f) {
        case LTR:
        case RTL:
            r.bottom(v);
            break;

        case TTB:
        case BTT:
            r.end(v);
            break;
        }
    }

    template <typename T>
    auto width(Math::Rect<T> r) -> T {
        switch (_f) {
        case LTR:
        case RTL:
            return r.width;

        case TTB:
        case BTT:
            return r.height;
        }
    }

    template <typename T>
    void width(Math::Rect<T> &r, T v) {
        switch (_f) {
        case LTR:
        case RTL:
            r.width = v;
            break;

        case TTB:
        case BTT:
            r.height = v;
            break;
        }
    }

    template <typename T>
    auto height(Math::Rect<T> r) -> T {
        switch (_f) {
        case LTR:
        case RTL:
            return r.height;

        case TTB:
        case BTT:
            return r.width;
        }
    }

    template <typename T>
    void height(Math::Rect<T> &r, T v) {
        switch (_f) {
        case LTR:
        case RTL:
            r.height = v;
            break;

        case TTB:
        case BTT:
            r.width = v;
            break;
        }
    }

    template <typename T>
    auto origin(Math::Rect<T> r) -> Math::Vec2<T> {
        return {start(r), top(r)};
    }

    template <typename T>
    void origin(Math::Rect<T> &r, Math::Vec2<T> v) {
        start(r, v.x);
        top(r, v.y);
    }

    template <typename T>
    auto center(Math::Rect<T> r) -> Math::Vec2<T> {
        return {start(r) + width(r) / 2, top(r) + height(r) / 2};
    }

    Flow apply(Flow flow) {
        _F table[4 * 4] = {LTR, RTL, TTB, BTT, RTL, LTR, TTB, BTT,
                           TTB, BTT, LTR, RTL, BTT, TTB, LTR, RTL};

        return table[_f * 4 + flow._f];
    }
};

} // namespace Karm::Ui
