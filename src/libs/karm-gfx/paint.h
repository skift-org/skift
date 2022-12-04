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
        float pos;
    };

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

    Gradiant &addStop(Color color, float pos) {
        _stops.pushBack({color, pos});
        return *this;
    }

    Color sample(float pos) const {
        if (_stops.len() == 0) {
            return BLACK;
        }

        if (_stops.len() == 1) {
            return _stops[0].color;
        }

        if (pos <= _stops[0].pos) {
            return _stops[0].color;
        }

        if (pos >= _stops[_stops.len() - 1].pos) {
            return _stops[_stops.len() - 1].color;
        }

        for (size_t i = 0; i < _stops.len() - 1; i++) {
            auto iPos = _stops[i].pos;
            auto jPos = _stops[i + 1].pos;

            if (pos >= iPos && pos <= jPos) {
                float t = (pos - iPos) / (jPos - iPos);
                auto iColor = _stops[i].color;
                auto jColor = _stops[i + 1].color;
                return iColor.lerpWith(jColor, t);
            }
        }

        return BLACK;
    }

    Color sample(Math::Vec2f pos) const {
        switch (_type) {
        case LINEAR:
            return sample(pos.x);
        case RADIAL:
            return sample(pos.len());
        case CONICAL:
            return sample(pos.angle());
        case DIAMOND:
            return sample(pos.x + pos.y);
        }
    }
};

struct Paint {
    Var<Color, Gradiant, Media::Image> _inner;

    Color sample(Math::Vec2f pos) const {
        return _inner.visit(Visitor{
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
