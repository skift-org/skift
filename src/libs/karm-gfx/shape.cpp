#include <karm-base/clamp.h>
#include <karm-base/try.h>

#include "shape.h"

namespace Karm::Gfx {

/* --- Common --------------------------------------------------------------- */

static void _createArc(Shape &shape, Math::Vec2f center, Math::Vec2f start, Math::Vec2f end, double startAngle, double delta, double radius, Math::Trans2f trans) {
    int devision = 32; // FIXME: determine this procedurally
    double step = delta / devision;
    for (int i = 0; i < devision; i++) {
        double sa = startAngle + step * i;
        double ea = startAngle + step * (i + 1);

        auto sp = i == 0 ? start : center + Math::Vec2f{radius * Math::cos(sa), radius * Math::sin(sa)};
        auto ep = i + 1 == devision ? end : center + Math::Vec2f{radius * Math::cos(ea), radius * Math::sin(ea)};

        shape.add(trans.apply({sp, ep}));
    }
}

/* --- Line Join ------------------------------------------------------------ */

static void _createJoinBevel(Shape &shape, Math::Edgef curr, Math::Edgef next, Math::Trans2f trans) {
    shape.add(trans.apply({curr.end, next.start}));
}

static void _createJoinMiter(Shape &shape, Math::Edgef curr, Math::Edgef next, Math::Vec2f corner, double width, Math::Trans2f trans) {
    auto currVec = curr.dir();
    auto nextVec = next.invDir();
    auto diffVec = next.start - curr.end;

    double mitterLimit = width * 4;
    auto c = nextVec.cross(currVec);

    if (Math::abs(c) < 0.001) {
        // parallel
        return;
    }

    auto j = nextVec.cross(diffVec) / c;
    auto v = curr.end + (currVec * j);

    if (j < 0 or (corner - v).lenSq() > mitterLimit * mitterLimit) {
        _createJoinBevel(shape, curr, next, trans);
        return;
    }

    shape.add(trans.apply({curr.end, v}));
    shape.add(trans.apply({v, next.start}));
}

static void _createJoinRound(Shape &shape, Math::Edgef curr, Math::Edgef next, Math::Vec2f corner, double radius, Math::Trans2f trans) {
    double startAngle = (curr.end - corner).angle();
    double endAngle = (next.start - corner).angle();

    if (startAngle > endAngle) {
        startAngle -= Math::TAU;
    }

    double delta = endAngle - startAngle;

    if (delta > Math::PI) {
        _createJoinBevel(shape, curr, next, trans);
        return;
    }

    _createArc(shape, corner, curr.end, next.start, startAngle, delta, radius, trans);
}

[[maybe_unused]] static void _createJoin(Shape &shape, StrokeStyle stroke, Math::Edgef curr, Math::Edgef next, Math::Vec2f corner, double radius, Math::Trans2f trans) {
    if (Math::epsilonEq(curr.end, next.start, 0.001)) {
        // HACK: avoid creating a degenerate edge
        return;
    }

    switch (stroke.join) {
    case BEVEL_JOIN:
        _createJoinBevel(shape, curr, next, trans);
        break;

    case MITER_JOIN:
        _createJoinMiter(shape, curr, next, corner, stroke.width, trans);
        break;

    case ROUND_JOIN:
        _createJoinRound(shape, curr, next, corner, radius, trans);
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

static void _createCapButt(Shape &shape, Cap cap, Math::Trans2f trans) {
    shape.add(trans.apply({cap.start, cap.end}));
}

static void _createCapSquare(Shape &shape, Cap cap, double width, Math::Trans2f trans) {
    auto e = Math::Edgef{cap.start, cap.end}.parallel(-width / 2);
    shape.add(trans.apply({cap.start, e.start}));
    shape.add(trans.apply(e));
    shape.add(trans.apply({e.end, cap.end}));
}

static void _createCapRound(Shape &shape, Cap cap, double width, Math::Trans2f trans) {
    double startAngle = (cap.start - cap.center).angle();
    double endAngle = (cap.end - cap.center).angle();

    if (startAngle > endAngle) {
        startAngle -= Math::TAU;
    }

    double delta = endAngle - startAngle;

    _createArc(shape, cap.center, cap.start, cap.end, startAngle, delta, width / 2, trans);
}

[[maybe_unused]] static void _createCap(Shape &shape, StrokeStyle stroke, Cap cap, Math::Trans2f trans) {
    switch (stroke.cap) {
    case BUTT_CAP:
        _createCapButt(shape, cap, trans);
        break;
    case SQUARE_CAP:
        _createCapSquare(shape, cap, stroke.width, trans);
        break;
    case ROUND_CAP:
        _createCapRound(shape, cap, stroke.width, trans);
        break;
    default:
        panic("unknown cap type");
    }
}

/* --- Public Api ----------------------------------------------------------- */

[[gnu::flatten]] void createStroke(Shape &shape, Path const &path, StrokeStyle stroke, Math::Trans2f trans) {
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

            shape.add(trans.apply(outerCurr));
            shape.add(trans.apply(innerCurr));

            if (i == 0 and not seg.close) {
                auto center = (innerCurr.end + outerCurr.start) / 2;
                _createCap(shape, stroke, {innerCurr.end, outerCurr.start, center}, trans);
            }

            if (i + 1 == l and not seg.close) {
                auto center = (outerCurr.end + innerCurr.start) / 2;
                _createCap(shape, stroke, {outerCurr.end, innerCurr.start, center}, trans);
            }

            if (seg.close or i + 1 != l) {
                Math::Edgef next = {
                    seg[(i + 1) % seg.len()],
                    seg[(i + 2) % seg.len()],
                };

                auto outerNext = outerDist > -0.001 ? next : next.parallel(outerDist);
                auto innerNext = innerDist < 0.001 ? next.swap() : next.parallel(innerDist).swap();

                if (outerDist < -0.001)
                    _createJoin(shape, stroke, outerCurr, outerNext, curr.end, Math::abs(outerDist), trans);

                if (innerDist > 0.001)
                    _createJoin(shape, stroke, innerNext, innerCurr, curr.end, Math::abs(innerDist), trans);
            }
        }
    }
}

void createSolid(Shape &shape, Path &path, Math::Trans2f trans) {
    for (auto seg : path.iterSegs()) {
        for (size_t i = 0; i < seg.len(); i++) {
            Math::Edgef e = {seg[i], seg[(i + 1) % seg.len()]};
            shape.add(trans.apply(e));
        }
    }
}

} // namespace Karm::Gfx
