#pragma once

#include <karm-base/var.h>
#include <karm-base/vec.h>
#include <karm-math/trans.h>
#include <karm-media/image.h>

#include "color.h"
#include "colors.h"

namespace Karm::Gfx {

struct Gradient {
    enum Type {
        LINEAR,
        RADIAL,
        CONICAL,
        DIAMOND,
    };

    using Buf = Array<Color, 256>;

    Type _type = LINEAR;
    Math::Vec2f _start = {0.5, 0.5};
    Math::Vec2f _end = {1, 1};
    Strong<Buf> _buf;

    struct _Builder {
        static constexpr isize LIMIT = 16;
        using Stop = Cons<Color, f64>;

        Type _type = LINEAR;
        Math::Vec2f _start = {0.5, 0.5};
        Math::Vec2f _end = {1, 1};
        InlineVec<Stop, LIMIT> _stops;

        _Builder(Type type) : _type(type) {}

        _Builder(Type type, Math::Vec2f start, Math::Vec2f end)
            : _type(type), _start(start), _end(end) {}

        _Builder &withStop(Color color, f64 pos) {
            _stops.pushBack({color, pos});
            return *this;
        }

        _Builder &withStart(Math::Vec2f start) {
            _start = start;
            return *this;
        }

        _Builder &withEnd(Math::Vec2f end) {
            _end = end;
            return *this;
        }

        _Builder &withHsv() {
            for (f64 i = 0; i <= 360; i += 60) {
                withStop(hsvToRgb({i, 1, 1}), i / 360.0);
            }
            return *this;
        }

        _Builder &withColors(Meta::Same<Color> auto... args) {
            Array colors = {args...};

            if (colors.len() == 1) {
                return withStop(colors[0], 0.5);
            }

            for (usize i = 0; i < colors.len(); i++) {
                withStop(colors[i], (f64)i / (colors.len() - 1));
            }

            return *this;
        }

        static Color lerp(Stop lhs, Stop rhs, f64 pos) {
            f64 t = (pos - lhs.cdr) / (rhs.cdr - lhs.cdr);
            return lhs.car.lerpWith(rhs.car, t);
        }

        Strong<Buf> bakeStops() {
            auto buf = makeStrong<Buf>();

            fill(mutSub(*buf), Gfx::BLACK);
            if (_stops.len() == 0) {
                return buf;
            }

            if (_stops.len() == 1) {
                fill(mutSub(*buf), _stops[0].car);
                return buf;
            }

            for (float j = 0; j < _stops[0].cdr * 256; j++) {
                if (_type == CONICAL) {
                    auto lhs = _stops[_stops.len() - 1];
                    lhs.cdr -= 1;
                    (*buf)[j] = lerp(lhs, _stops[0], j / 256.0);
                } else {
                    (*buf)[j] = _stops[0].car;
                }
            }

            for (usize i = 0; i < _stops.len() - 1; i++) {
                auto iPos = _stops[i].cdr;
                auto jPos = _stops[i + 1].cdr;

                for (f64 j = iPos * 256; j < jPos * 256; j++) {
                    (*buf)[j] = lerp(_stops[i], _stops[i + 1], j / 256.0);
                }
            }

            for (f64 j = _stops[_stops.len() - 1].cdr * 256; j < 256; j++) {
                if (_type == CONICAL) {
                    auto rhs = _stops[0];
                    rhs.cdr += 1;
                    (*buf)[j] = lerp(_stops[_stops.len() - 1], rhs, j / 256.0);
                } else {
                    (*buf)[j] = _stops[_stops.len() - 1].car;
                }
            }

            return buf;
        }

        Gradient bake() {
            return {_type, _start, _end, bakeStops()};
        }
    };

    static _Builder linear() {
        return _Builder{LINEAR, {0, 0}, {1, 1}};
    }

    static _Builder hsv() {
        return hlinear()
            .withHsv();
    }

    static _Builder vlinear() {
        return _Builder{LINEAR, {0.5, 0}, {0.5, 1}};
    }

    static _Builder hlinear() {
        return _Builder{LINEAR, {0, 0.5}, {1, 0.5}};
    }

    static _Builder radial() {
        return _Builder{RADIAL, {0.5, 0.5}, {1, 0.5}};
    }

    static _Builder conical() {
        return _Builder{CONICAL, {0.5, 0.5}, {1, 0.5}};
    }

    static _Builder diamond() {
        return _Builder{DIAMOND, {0.5, 0.5}, {1, 0.5}};
    }

    Gradient(Type type, Math::Vec2f start, Math::Vec2f end, Strong<Buf> buf)
        : _type(type), _start(start), _end(end), _buf(buf) {}

    ALWAYS_INLINE f64 transform(Math::Vec2f pos) const {
        pos = pos - _start;
        pos = pos.rotate(-(_end - _start).angle());
        f64 scale = (_end - _start).len();
        pos = pos / scale;

        switch (_type) {
        case LINEAR:
            return pos.x;

        case RADIAL:
            return pos.len();

        case CONICAL:
            return (pos.angle() + Math::PI) / Math::TAU;

        case DIAMOND:
            return Math::abs(pos.x) + Math::abs(pos.y);
        }
    }

    ALWAYS_INLINE Color sample(Math::Vec2f pos) const {
        auto p = transform(pos);
        return (*_buf)[clamp(usize(p * 255), 0uz, 255uz)];
    }
};

using _Paints = Var<
    Color,
    Gradient,
    Media::Image>;

struct Paint : public _Paints {
    using _Paints::_Paints;

    ALWAYS_INLINE Color sample(Math::Vec2f pos) const {
        return visit([&](auto const &p) {
            return p.sample(pos);
        });
    }
};

} // namespace Karm::Gfx
