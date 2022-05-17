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
    Math::Vec2<T> vec() {
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
    T start(Math::Rect<T> r) {
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
    T end(Math::Rect<T> r) {
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
    T top(Math::Rect<T> r) {
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
    T bottom(Math::Rect<T> r) {
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
    T width(Math::Rect<T> r) {
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
    T height(Math::Rect<T> r) {
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
    Math::Vec2<T> origin(Math::Rect<T> r) {
        return {start(r), top(r)};
    }

    template <typename T>
    void origin(Math::Rect<T> &r, Math::Vec2<T> v) {
        start(r, v.x);
        top(r, v.y);
    }

    template <typename T>
    Math::Vec2<T> center(Math::Rect<T> r) {
        return {start(r) + width(r) / 2, top(r) + height(r) / 2};
    }

    Flow apply(Flow flow) {
        _F table[4 * 4] = {LTR, RTL, TTB, BTT, RTL, LTR, TTB, BTT,
                           TTB, BTT, LTR, RTL, BTT, TTB, LTR, RTL};

        return table[_f * 4 + flow._f];
    }
};

struct Dock {
    enum _D {
        NONE,
        FILL,
        START,
        TOP,
        END,
        BOTTOM,
    };

    _D _d;

    Flow flow() const {
        switch (_d) {
        case NONE:
        case FILL:
            return Flow::LTR;
        case START:
            return Flow::LTR;
        case TOP:
            return Flow::TTB;
        case END:
            return Flow::RTL;
        case BOTTOM:
            return Flow::BTT;
        }
    }

    template <typename T>
    Math::Rect<T> apply(Flow flow, Math::Rect<T> child, Math::Rect<T> &parent) {
        flow = flow.apply(this->flow());

        switch (_d) {
        case NONE:
            break;

        case FILL:
            child = *parent;
            break;

        default:
            child = flow.origin(child, flow.origin(*parent));
            child = flow.height(child, flow.height(*parent));
            *parent = flow.start(*parent, flow.end(child));
            break;
        }

        return child;
    }
};

struct Gravity {
    enum _G {
        NONE = (0 << 0),

        START = (1 << 0),
        END = (1 << 1),
        TOP = (1 << 2),
        BOTTOM = (1 << 3),
        VSTRETCH = (1 << 4),
        HSTRETCH = (1 << 5),
        VCENTER = (1 << 6),
        HCENTER = (1 << 7),

        COVER = (1 << 8),
        FIT = (1 << 9),

        CENTER = (VCENTER | HCENTER),
        VFILL = (VSTRETCH | START),
        HFILL = (HSTRETCH | TOP),
        FILL = (VFILL | HFILL),
    };

    uint32_t _g;

    constexpr Gravity() : _g(NONE) {}

    constexpr Gravity(uint32_t flags) : _g(flags) {}

    template <typename T>
    Math::Rect<T> apply(Flow flow, Math::Rect<T> child, Math::Rect<T> parent) {
        if (_g == NONE) {
            return child;
        }

        if (_g & COVER) {
            double scale = max(parent.size() / child.size());
            child.size(child.size() * scale);
        }

        if (_g & FIT) {
            double scale = min(parent.size() / child.size());
            child.size(child.size() * scale);
        }

        if (_g & START) {
            flow.start(child, flow.start(parent));
        }

        if (_g & END) {
            flow.end(child, flow.end(parent));
        }

        if (_g & TOP) {
            flow.top(child, flow.top(parent));
        }

        if (_g & BOTTOM) {
            flow.bottom(child, flow.bottom(parent));
        }

        if (_g & VSTRETCH) {
            flow.width(child, flow.width(parent));
        }

        if (_g & HSTRETCH) {
            flow.height(child, flow.height(parent));
        }

        if (_g & VCENTER) {
            flow.top(child, flow.top(parent) + flow.height(parent) / 2 - flow.height(child) / 2);
        }

        if (_g & HCENTER) {
            flow.start(child, flow.start(parent) + flow.width(parent) / 2 - flow.width(child) / 2);
        }

        return child;
    }
};

} // namespace Karm::Ui
