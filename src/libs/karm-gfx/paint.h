#pragma once

#include <karm-base/var.h>
#include <karm-base/vec.h>
#include <karm-media/image.h>

#include "color.h"
#include "colors.h"

namespace Karm::Gfx {

struct Gradiant {
    static constexpr int MAX_STOPS = 16;

    struct Stop {
        Color color;
        double pos;
    };

    static Color lerp(Stop lhs, Stop rhs, double pos) {
        double t = (pos - lhs.pos) / (rhs.pos - lhs.pos);
        return lhs.color.lerpWith(rhs.color, t);
    }

    enum Type {
        LINEAR,
        RADIAL,
        CONICAL,
        DIAMOND,
    };

    Type _type = LINEAR;
    Math::Vec2f _start = {0.5, 0.5};
    Math::Vec2f _end = {1, 1};
    InlineVec<Stop, MAX_STOPS> _stops = {};

    static Gradiant linear() {
        return Gradiant{LINEAR, {0, 0.5}, {1, 0.5}};
    }

    static Gradiant radial() {
        return Gradiant{RADIAL, {0.5, 0.5}, {1, 0.5}};
    }

    static Gradiant conical() {
        return Gradiant{CONICAL, {0.5, 0.5}, {1, 0.5}};
    }

    static Gradiant diamond() {
        return Gradiant{DIAMOND, {0.5, 0.5}, {1, 0.5}};
    }

    Gradiant &addStop(Color color, double pos) {
        _stops.pushBack({color, pos});
        return *this;
    }

    Color sample(double pos, bool wrapAround = false) const {
        if (_stops.len() == 0) {
            return BLACK;
        }

        if (_stops.len() == 1) {
            return _stops[0].color;
        }

        if (pos <= _stops[0].pos) {
            if (wrapAround) {
                auto lhs = _stops[_stops.len() - 1];
                lhs.pos -= 1;

                return lerp(lhs, _stops[0], pos);
            }

            return _stops[0].color;
        }

        if (pos >= _stops[_stops.len() - 1].pos) {
            if (wrapAround) {
                auto rhs = _stops[0];
                rhs.pos += 1;

                return lerp(_stops[_stops.len() - 1], rhs, pos);
            }
            return _stops[_stops.len() - 1].color;
        }

        for (size_t i = 0; i < _stops.len() - 1; i++) {
            auto iPos = _stops[i].pos;
            auto jPos = _stops[i + 1].pos;

            if (pos >= iPos && pos <= jPos) {
                return lerp(_stops[i], _stops[i + 1], pos);
            }
        }

        return BLACK;
    }

    Color sample(Math::Vec2f pos) const {
        pos = pos - _start;
        switch (_type) {
        case LINEAR:
            return sample(pos.x);
        case RADIAL:
            return sample(pos.len());
        case CONICAL: {
            double offset = (_end - _start).angle();
            pos = pos.rotate(-offset);
            return sample((pos.angle() + Math::PI) / Math::TAU, true);
        }
        case DIAMOND:
            return sample((pos.x + pos.y) / 2);
        }
    }
};

struct Paint : public Var<Color, Gradiant, Media::Image> {
    using Var::Var;

    Color sample(Math::Vec2f pos) const {
        return visit(Visitor{
            [&](Color color) {
                return color;
            },
            [&](Gradiant gradiant) {
                return gradiant.sample(pos);
            },
            [&](Media::Image image) {
                auto w = image.width();
                auto h = image.height();
                Surface s = image;
                return s.loadClamped(Math::Vec2i(pos.x * w, pos.y * h));
            }});
    }
};

} // namespace Karm::Gfx
