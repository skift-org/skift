#pragma once

#include <karm-math/path.h>
#include <karm-math/poly.h>

#include "fill.h"

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
    Fill fill = BLACK;
    f64 width{1};
    StrokeAlign align = CENTER_ALIGN;
    StrokeCap cap = BUTT_CAP;
    StrokeJoin join = BEVEL_JOIN;

    auto& withFill(Fill p) {
        fill = p;
        return *this;
    }

    auto& withWidth(f64 w) {
        width = w;
        return *this;
    }

    auto& withAlign(StrokeAlign a) {
        align = a;
        return *this;
    }

    auto& withCap(StrokeCap c) {
        cap = c;
        return *this;
    }

    auto& withJoin(StrokeJoin j) {
        join = j;
        return *this;
    }
};

inline Stroke stroke(auto... args) {
    return {args...};
}

void createStroke(Math::Polyf& poly, Math::Path const& path, Stroke stroke);

void createSolid(Math::Polyf& poly, Math::Path const& path);

} // namespace Karm::Gfx
