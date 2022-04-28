#pragma once

#include <karm-base/clamp.h>
#include <karm-base/std.h>

namespace Karm::Math {

template <typename T>
union V2 {
    struct {
        T x, y;
    };

    struct {
        T width, height;
    };

    T _els[2];

    auto min() const -> T {
        return Base::min(x, y);
    }

    auto max() const -> T {
        return Base::max(x, y);
    }

    auto operator[](int i) const -> T { return _els[i]; }

    auto operator+(T const &other) const -> V2 { return {x + other, y + other}; }

    auto operator-(T const &other) const -> V2 { return {x - other, y - other}; }

    auto operator*(T const &other) const -> V2 { return {x * other, y * other}; }

    auto operator/(T const &other) const -> V2 { return {x / other, y / other}; }

    auto operator+(V2 const &other) const -> V2 { return {x + other.x, y + other.y}; }

    auto operator-(V2 const &other) const -> V2 { return {x - other.x, y - other.y}; }

    auto operator*(V2 const &other) const -> V2 { return {x * other.x, y * other.y}; }

    auto operator/(V2 const &other) const -> V2 { return {x / other.x, y / other.y}; }
};

template <typename T>
union V3 {
    struct {
        T x, y, z;
    };

    struct {
        V2<T> xy;
        T _z;
    };

    T _els[3];

    auto min() const -> T {
        return Base::min(x, y, z);
    }

    auto max() const -> T {
        return Base::max(x, y, z);
    }

    auto operator[](int i) -> T { return _els[i]; }

    auto operator+(T const &other) const -> V3 { return {x + other, y + other, z + other}; }

    auto operator-(T const &other) const -> V3 { return {x - other, y - other, z - other}; }

    auto operator*(T const &other) const -> V3 { return {x * other, y * other, z * other}; }

    auto operator/(T const &other) const -> V3 { return {x / other, y / other, z / other}; }

    auto operator+(V3 const &other) const -> V3 { return {x + other.x, y + other.y, z + other.z}; }

    auto operator-(V3 const &other) const -> V3 { return {x - other.x, y - other.y, z - other.z}; }

    auto operator*(V3 const &other) const -> V3 { return {x * other.x, y * other.y, z * other.z}; }

    auto operator/(V3 const &other) const -> V3 { return {x / other.x, y / other.y, z / other.z}; }
};

template <typename T>
union V4 {
    struct {
        T x, y, z, w;
    };

    struct {
        V3<T> xyz;
        T _w;
    };

    struct {
        V2<T> xy;
        V2<T> zw;
    };

    T _els[4];

    auto min() const -> T {
        return Base::min(x, y, z, w);
    }

    auto max() const -> T {
        return Base::max(x, y, z, w);
    }

    auto operator[](int i) -> T { return _els[i]; }

    auto operator+(T const &other) const -> V4 { return {x + other, y + other, z + other, w + other}; }

    auto operator-(T const &other) const -> V4 { return {x - other, y - other, z - other, w - other}; }

    auto operator*(T const &other) const -> V4 { return {x * other, y * other, z * other, w * other}; }

    auto operator/(T const &other) const -> V4 { return {x / other, y / other, z / other, w / other}; }

    auto operator+(V4 const &other) const -> V4 { return {x + other.x, y + other.y, z + other.z, w + other.w}; }

    auto operator-(V4 const &other) const -> V4 { return {x - other.x, y - other.y, z - other.z, w - other.w}; }

    auto operator*(V4 const &other) const -> V4 { return {x * other.x, y * other.y, z * other.z, w * other.w}; }

    auto operator/(V4 const &other) const -> V4 { return {x / other.x, y / other.y, z / other.z, w / other.w}; }
};

template <typename T>
union Rect {
    struct {
        T x, y, width, height;
    };

    struct {
        V2<T> xy;
        V2<T> wh;
    };

    T _els[4];

    auto min() const -> V2<T> { return {x, y}; }

    auto max() const -> V2<T> { return {x + width, y + height}; }

    auto start() const -> T { return x; }

    void start(T value) {
        T d = value - x;
        x += d;
        width -= d;
    }

    auto end() const -> T { return x + width; }

    void end(T value) {
        T d = value - (x + width);
        width += d;
    }

    auto top() const -> T { return y; }

    void top(T value) {
        T d = value - y;
        y += d;
        height -= d;
    }

    auto bottom() const -> T { return y + height; }

    void bottom(T value) {
        T d = value - (y + height);
        height += d;
    }

    auto center() const -> V2<T> { return {x + width / 2, y + height / 2}; }

    auto size() const -> V2<T> { return {width, height}; }

    auto xw() const -> V2<T> { return {x, x + width}; }

    void xw(V2<T> const &v) {
        x = v.x;
        width = v.y - v.x;
    }

    auto yh() const -> V2<T> { return {y, y + height}; }

    void yh(V2<T> const &v) {
        y = v.x;
        height = v.y - v.x;
    }

    auto operator[](int i) -> T { return _els[i]; }

    auto operator[](int i) const -> T { return _els[i]; }
};

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
    auto vec() -> V2<T> {
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
    auto start(Rect<T> r) -> T {
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
    void start(Rect<T> &r, T v) {
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
    auto end(Rect<T> r) -> T {
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
    void end(Rect<T> &r, T v) {
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
    auto top(Rect<T> r) -> T {
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
    void top(Rect<T> &r, T v) {
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
    auto bottom(Rect<T> r) -> T {
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
    void bottom(Rect<T> &r, T v) {
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
    auto width(Rect<T> r) -> T {
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
    void width(Rect<T> &r, T v) {
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
    auto height(Rect<T> r) -> T {
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
    void height(Rect<T> &r, T v) {
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
    auto origin(Rect<T> r) -> V2<T> {
        return {start(r), top(r)};
    }

    template <typename T>
    auto center(Rect<T> r) -> V2<T> {
        return {start(r) + width(r) / 2, top(r) + height(r) / 2};
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
    Rect<T> apply(Flow flow, Rect<T> child, Rect<T> parent) {
        if (_g == NONE) {
            return child;
        }

        if (_g & COVER) {
            double scale = Base::max(parent.size() / child.size());
            child.size(child.size() * scale);
        }

        if (_g & FIT) {
            double scale = Base::min(parent.size() / child.size());
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
    Rect<T> apply(Flow flow, Rect<T> child, Rect<T> &parent) {
    }
};

} // namespace Karm::Math
