#include <karm-base/clamp.h>
#include <karm-base/try.h>

#include "shape.h"

namespace Karm::Gfx {

/* --- Common --------------------------------------------------------------- */

static void _createArc(Shape &shape, Math::Vec2f center, Math::Vec2f start, Math::Vec2f end, f64 startAngle, f64 delta, f64 radius) {
    isize devision = 32; // FIXME: determine this procedurally
    f64 step = delta / devision;
    for (isize i = 0; i < devision; i++) {
        f64 sa = startAngle + step * i;
        f64 ea = startAngle + step * (i + 1);

        auto sp = i == 0 ? start : center + Math::Vec2f{radius * Math::cos(sa), radius * Math::sin(sa)};
        auto ep = i + 1 == devision ? end : center + Math::Vec2f{radius * Math::cos(ea), radius * Math::sin(ea)};

        shape.add({sp, ep});
    }
}

/* --- Line Join ------------------------------------------------------------ */

static void _createJoinBevel(Shape &shape, Math::Edgef curr, Math::Edgef next) {
    shape.add({curr.end, next.start});
}

static void _createJoinMiter(Shape &shape, Math::Edgef curr, Math::Edgef next, Math::Vec2f corner, f64 width) {
    auto currVec = curr.dir();
    auto nextVec = next.invDir();
    auto diffVec = next.start - curr.end;

    f64 mitterLimit = width * 4;
    auto c = nextVec.cross(currVec);

    if (Math::abs(c) < 0.001) {
        // parallel
        return;
    }

    auto j = nextVec.cross(diffVec) / c;
    auto v = curr.end + (currVec * j);

    if (j < 0 or (corner - v).lenSq() > mitterLimit * mitterLimit) {
        _createJoinBevel(shape, curr, next);
        return;
    }

    shape.add({curr.end, v});
    shape.add({v, next.start});
}

static void _createJoinRound(Shape &shape, Math::Edgef curr, Math::Edgef next, Math::Vec2f corner, f64 radius) {
    f64 startAngle = (curr.end - corner).angle();
    f64 endAngle = (next.start - corner).angle();

    if (startAngle > endAngle) {
        startAngle -= Math::TAU;
    }

    f64 delta = endAngle - startAngle;

    if (delta > Math::PI) {
        _createJoinBevel(shape, curr, next);
        return;
    }

    _createArc(shape, corner, curr.end, next.start, startAngle, delta, radius);
}

[[maybe_unused]] static void _createJoin(Shape &shape, StrokeStyle stroke, Math::Edgef curr, Math::Edgef next, Math::Vec2f corner, f64 radius) {
    // Make sure that the edge is not degenerate
    if (Math::Edgef{curr.end, next.start}.degenerated()) {
        return;
    }

    switch (stroke.join) {
    case BEVEL_JOIN:
        _createJoinBevel(shape, curr, next);
        break;

    case MITER_JOIN:
        _createJoinMiter(shape, curr, next, corner, stroke.width);
        break;

    case ROUND_JOIN:
        _createJoinRound(shape, curr, next, corner, radius);
        break;

    default:
        panic("unknown join type");
    }
}

/* --- Line Cap ------------------------------------------------------------- */

struct Cap {
    Math::Vec2f start;
    Math::Vec2f end;
    Math::Vec2f center;
};

static void _createCapButt(Shape &shape, Cap cap) {
    shape.add({cap.start, cap.end});
}

static void _createCapSquare(Shape &shape, Cap cap, f64 width) {
    auto e = Math::Edgef{cap.start, cap.end}.parallel(-width / 2);
    shape.add({cap.start, e.start});
    shape.add(e);
    shape.add({e.end, cap.end});
}

static void _createCapRound(Shape &shape, Cap cap, f64 width) {
    f64 startAngle = (cap.start - cap.center).angle();
    f64 endAngle = (cap.end - cap.center).angle();

    if (startAngle > endAngle) {
        startAngle -= Math::TAU;
    }

    f64 delta = endAngle - startAngle;

    _createArc(shape, cap.center, cap.start, cap.end, startAngle, delta, width / 2);
}

[[maybe_unused]] static void _createCap(Shape &shape, StrokeStyle stroke, Cap cap) {
    switch (stroke.cap) {
    case BUTT_CAP:
        _createCapButt(shape, cap);
        break;
    case SQUARE_CAP:
        _createCapSquare(shape, cap, stroke.width);
        break;
    case ROUND_CAP:
        _createCapRound(shape, cap, stroke.width);
        break;
    default:
        panic("unknown cap type");
    }
}

/* --- Public Api ----------------------------------------------------------- */

[[gnu::flatten]] void createStroke(Shape &shape, Path const &path, StrokeStyle stroke) {
    f64 outerDist = 0;

    if (stroke.align == CENTER_ALIGN) {
        outerDist = -stroke.width / 2;
    } else if (stroke.align == OUTSIDE_ALIGN) {
        outerDist = -stroke.width;
    }

    f64 innerDist = outerDist + stroke.width;

    for (auto seg : path.iterSegs()) {
        auto l = seg.close ? seg.len() : seg.len() - 1;

        for (usize i = 0; i < l; i++) {
            Math::Edgef curr = {seg[i], seg[(i + 1) % seg.len()]};

            if (curr.degenerated()) {
                continue;
            }

            auto outerCurr = curr.parallel(outerDist);
            auto innerCurr = curr.parallel(innerDist).swap();

            shape.add(outerCurr);
            shape.add(innerCurr);

            if (i == 0 and not seg.close) {
                auto center = (innerCurr.end + outerCurr.start) / 2;
                _createCap(shape, stroke, {innerCurr.end, outerCurr.start, center});
            }

            if (i + 1 == l and not seg.close) {
                auto center = (outerCurr.end + innerCurr.start) / 2;
                _createCap(shape, stroke, {outerCurr.end, innerCurr.start, center});
            }

            if (seg.close or i + 1 != l) {
                Math::Edgef next = {
                    seg[(i + 1) % seg.len()],
                    seg[(i + 2) % seg.len()],
                };

                // Make sure that the edge is not degenerate
                if (next.degenerated()) {
                    continue;
                }

                auto outerNext = outerDist > -0.001 ? next : next.parallel(outerDist);
                auto innerNext = innerDist < 0.001 ? next.swap() : next.parallel(innerDist).swap();

                if (outerDist < -0.001)
                    _createJoin(shape, stroke, outerCurr, outerNext, curr.end, Math::abs(outerDist));

                if (innerDist > 0.001)
                    _createJoin(shape, stroke, innerNext, innerCurr, curr.end, Math::abs(innerDist));
            }
        }
    }
}

void createSolid(Shape &shape, Path &path) {
    for (auto seg : path.iterSegs()) {
        for (usize i = 0; i < seg.len(); i++) {
            Math::Edgef e = {seg[i], seg[(i + 1) % seg.len()]};
            shape.add(e);
        }
    }
}

} // namespace Karm::Gfx
