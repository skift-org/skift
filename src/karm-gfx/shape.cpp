#include <karm-base/clamp.h>
#include <karm-base/try.h>

#include "shape.h"

namespace Karm::Gfx {

/* --- Common --------------------------------------------------------------- */

static void _createArc(Shape &shape, Math::Vec2f center, double startAngle, double delta, double radius) {
    int devision = 32; // FIXME: determine this procedurally
    double step = delta / devision;
    for (int i = 0; i < devision; i++) {
        double sa = startAngle + step * i;
        double ea = startAngle + step * (i + 1);
        shape.add({center + Math::Vec2f{radius * Math::cos(sa), radius * Math::sin(sa)},
                   center + Math::Vec2f{radius * Math::cos(ea), radius * Math::sin(ea)}});
    }
}

/* --- Line Join ------------------------------------------------------------ */

static void _createJoinBevel(Shape &shape, Math::Edgef curr, Math::Edgef next) {
    shape.add({curr.end, next.start});
}

static void _createJoinMiter(Shape &shape, Math::Edgef curr, Math::Edgef next, Math::Vec2f corner, float width) {
    auto currVec = curr.dir();
    auto nextVec = next.invDir();
    auto diffVec = next.start - curr.end;

    double mitterLimit = width * 4;
    auto c = nextVec.cross(currVec);

    if (abs(c) < 0.001) {
        // parallel
        return;
    }

    auto j = nextVec.cross(diffVec) / c;
    auto v = curr.end + (currVec * j);

    if (j < 0 || (corner - v).lenSq() > mitterLimit * mitterLimit) {
        _createJoinBevel(shape, curr, next);
        return;
    }

    shape.add({curr.end, v});
    shape.add({v, next.start});
}

static void _createJoinRound(Shape &shape, Math::Edgef curr, Math::Edgef next, Math::Vec2f corner, float radius) {
    double startAngle = (curr.end - corner).angle();
    double endAngle = (next.start - corner).angle();

    if (startAngle > endAngle) {
        startAngle -= Math::TAU;
    }

    double delta = endAngle - startAngle;

    if (delta > Math::PI) {
        _createJoinBevel(shape, curr, next);
        return;
    }

    _createArc(shape, corner, startAngle, delta, radius);
}

[[maybe_unused]] static void _createJoin(Shape &shape, Stroke stroke, Math::Edgef curr, Math::Edgef next, Math::Vec2f corner, float radius) {
    if (Math::epsilonEq(curr.end, next.start, 0.001)) {
        // HACK: avoid creating a degenerate edge
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

static void _createCapButt(Shape &shape, Math::Vec2f start, Math::Vec2f end) {
    shape.add({start, end});
}

static void _createCapSquare(Shape &shape, Math::Vec2f start, Math::Vec2f end, float width) {
    auto e = Math::Edgef{start, end}.parallel(-width / 2);
    shape.add({start, e.start});
    shape.add(e);
    shape.add({e.end, end});
}

static void _createCapRound(Shape &shape, Math::Vec2f start, Math::Vec2f end, Math::Vec2f center, float width) {
    double startAngle = (start - center).angle();
    double endAngle = (end - center).angle();

    if (startAngle > endAngle) {
        startAngle -= Math::TAU;
    }

    double delta = endAngle - startAngle;

    _createArc(shape, center, startAngle, delta, width / 2);
}

[[maybe_unused]] static void _createCap(Shape &shape, Stroke stroke, Math::Vec2f start, Math::Vec2f end, Math::Vec2f center) {
    switch (stroke.cap) {
    case BUTT_CAP:
        _createCapButt(shape, start, end);
        break;
    case SQUARE_CAP:
        _createCapSquare(shape, start, end, stroke.width);
        break;
    case ROUND_CAP:
        _createCapRound(shape, start, end, center, stroke.width);
        break;
    default:
        panic("unknown cap type");
    }
}

/* --- Public Api ----------------------------------------------------------- */

void createStroke(Shape &shape, Path const &path, Stroke stroke) {
    double outerDist = 0;

    if (stroke.align == CENTER_ALIGN) {
        outerDist = -stroke.width / 2;
    } else if (stroke.align == OUTSIDE_ALIGN) {
        outerDist = -stroke.width;
    }

    double innerDist = outerDist + stroke.width;

    for (auto seg : path.iterSegs()) {
        auto l = seg.close ? seg.len() : seg.len() - 1;

        for (size_t i = 0; i < l; i++) {
            Math::Edgef curr = {seg[i], seg[(i + 1) % seg.len()]};

            auto outerCurr = curr.parallel(outerDist);
            auto innerCurr = curr.parallel(innerDist).swap();

            shape.add(outerCurr);
            shape.add(innerCurr);

            if (i == 0 && !seg.close) {
                auto center = (innerCurr.end + outerCurr.start) / 2;
                _createCap(shape, stroke, innerCurr.end, outerCurr.start, center);
            }

            if (i + 1 == l && !seg.close) {
                auto center = (outerCurr.end + innerCurr.start) / 2;
                _createCap(shape, stroke, outerCurr.end, innerCurr.start, center);
            }

            if (seg.close || i + 1 != l) {
                Math::Edgef next = {
                    seg[(i + 1) % seg.len()],
                    seg[(i + 2) % seg.len()],
                };

                auto outerNext = next.parallel(outerDist);
                auto innerNext = next.parallel(innerDist).swap();

                if (outerDist < -0.001)
                    _createJoin(shape, stroke, outerCurr, outerNext, curr.end, abs(outerDist));

                if (innerDist > 0.001)
                    _createJoin(shape, stroke, innerNext, innerCurr, curr.end, abs(innerDist));
            }
        }
    }
}

void createSolid(Shape &shape, Path &path) {
    for (auto seg : path.iterSegs()) {
        for (size_t i = 0; i < seg.len(); i++) {
            shape.add({seg[i], seg[(i + 1) % seg.len()]});
        }
    }
}

} // namespace Karm::Gfx
