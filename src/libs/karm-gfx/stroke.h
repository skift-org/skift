#pragma once

#include <karm-math/poly.h>

#include "paint.h"
#include "path.h"

namespace Karm::Gfx {

enum StrokeAlign {
    CENTER_ALIGN,
    INSIDE_ALIGN,
    OUTSIDE_ALIGN,
};

enum StrokeCap {
    BUTT_CAP,
    SQUARE_CAP,
    ROUND_CAP,
};

enum StrokeJoin {
    BEVEL_JOIN,
    MITER_JOIN,
    ROUND_JOIN,
};

using enum StrokeAlign;
using enum StrokeCap;
using enum StrokeJoin;

struct Stroke {
    Paint paint;
    f64 width{1};
    StrokeAlign align{};
    StrokeCap cap{};
    StrokeJoin join{};

    auto &withPaint(Paint p) {
        paint = p;
        return *this;
    }

    auto &withWidth(f64 w) {
        width = w;
        return *this;
    }

    auto &withAlign(StrokeAlign a) {
        align = a;
        return *this;
    }

    auto &withCap(StrokeCap c) {
        cap = c;
        return *this;
    }

    auto &withJoin(StrokeJoin j) {
        join = j;
        return *this;
    }
};

inline Stroke stroke(auto... args) {
    return {args...};
}

void createStroke(Math::Polyf &poly, Path const &path, Stroke stroke);

void createSolid(Math::Polyf &poly, Path const &path);

} // namespace Karm::Gfx
