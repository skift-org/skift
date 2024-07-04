#pragma once

#include <karm-math/radius.h>
#include <karm-math/vec.h>

#include "paint.h"

namespace Karm::Gfx {

// MARK: Stroke Style ----------------------------------------------------------

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

struct StrokeStyle {
    Paint paint;
    f64 width{1};
    StrokeAlign align{};
    StrokeCap cap{};
    StrokeJoin join{};

    StrokeStyle(Paint c = WHITE) : paint(c) {}

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

inline StrokeStyle stroke(auto... args) {
    return {args...};
}

} // namespace Karm::Gfx
