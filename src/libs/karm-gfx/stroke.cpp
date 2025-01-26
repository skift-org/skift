#include "stroke.h"

namespace Karm::Gfx {

// MARK: Common ----------------------------------------------------------------

static void _createArc(Math::Polyf& poly, Math::Vec2f center, Math::Vec2f start, Math::Vec2f end, f64 startAngle, f64 delta, f64 radius) {
    isize divs = 32; // FIXME: determine this procedurally
    f64 step = delta / divs;
    for (isize i = 0; i < divs; i++) {
        f64 sa = startAngle + step * i;
        f64 ea = startAngle + step * (i + 1);

        auto sp = i == 0 ? start : center + Math::Vec2f{radius * Math::cos(sa), radius * Math::sin(sa)};
        auto ep = i + 1 == divs ? end : center + Math::Vec2f{radius * Math::cos(ea), radius * Math::sin(ea)};

        poly.pushBack({sp, ep});
    }
}

// MARK: Line Join -------------------------------------------------------------

static void _createJoinBevel(Math::Polyf& poly, Math::Edgef curr, Math::Edgef next) {
    poly.pushBack({curr.end, next.start});
}

static void _createJoinMiter(Math::Polyf& poly, Math::Edgef curr, Math::Edgef next, Math::Vec2f corner, f64 width) {
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
        _createJoinBevel(poly, curr, next);
        return;
    }

    poly.pushBack({curr.end, v});
    poly.pushBack({v, next.start});
}

static void _createJoinRound(Math::Polyf& poly, Math::Edgef curr, Math::Edgef next, Math::Vec2f corner, f64 radius) {
    f64 startAngle = (curr.end - corner).angle();
    f64 endAngle = (next.start - corner).angle();

    if (startAngle > endAngle) {
        startAngle -= Math::TAU;
    }

    f64 delta = endAngle - startAngle;

    if (delta > Math::PI) {
        _createJoinBevel(poly, curr, next);
        return;
    }

    _createArc(poly, corner, curr.end, next.start, startAngle, delta, radius);
}

static void _createJoin(Math::Polyf& poly, Stroke stroke, Math::Edgef curr, Math::Edgef next, Math::Vec2f corner, f64 radius) {
    // Make sure that the edge is not degenerate
    if (Math::Edgef{curr.end, next.start}.degenerated())
        return;

    switch (stroke.join) {
    case BEVEL_JOIN:
        _createJoinBevel(poly, curr, next);
        break;

    case MITER_JOIN:
        _createJoinMiter(poly, curr, next, corner, stroke.width);
        break;

    case ROUND_JOIN:
        _createJoinRound(poly, curr, next, corner, radius);
        break;

    default:
        panic("unknown join type");
    }
}

// MARK: Line Cap --------------------------------------------------------------

struct Cap {
    Math::Vec2f start;
    Math::Vec2f end;
    Math::Vec2f center;
};

static void _createCapButt(Math::Polyf& poly, Cap cap) {
    poly.pushBack({cap.start, cap.end});
}

static void _createCapSquare(Math::Polyf& poly, Cap cap, f64 width) {
    auto e = Math::Edgef{cap.start, cap.end}.offset(-width / 2);
    poly.pushBack({cap.start, e.start});
    poly.pushBack(e);
    poly.pushBack({e.end, cap.end});
}

static void _createCapRound(Math::Polyf& poly, Cap cap, f64 width) {
    f64 startAngle = (cap.start - cap.center).angle();
    f64 endAngle = (cap.end - cap.center).angle();

    if (startAngle > endAngle) {
        startAngle -= Math::TAU;
    }

    f64 delta = endAngle - startAngle;

    _createArc(poly, cap.center, cap.start, cap.end, startAngle, delta, width / 2);
}

static void _createCap(Math::Polyf& poly, Stroke stroke, Cap cap) {
    switch (stroke.cap) {
    case BUTT_CAP:
        _createCapButt(poly, cap);
        break;
    case SQUARE_CAP:
        _createCapSquare(poly, cap, stroke.width);
        break;
    case ROUND_CAP:
        _createCapRound(poly, cap, stroke.width);
        break;
    default:
        panic("unknown cap type");
    }
}

// MARK: Public Api ------------------------------------------------------------

void createStroke(Math::Polyf& poly, Math::Path const& path, Stroke stroke) {
    f64 outerDist = 0;

    if (stroke.align == CENTER_ALIGN) {
        outerDist = -stroke.width / 2;
    } else if (stroke.align == OUTSIDE_ALIGN) {
        outerDist = -stroke.width;
    }

    f64 innerDist = outerDist + stroke.width;

    for (auto contour : path.iterContours()) {
        auto l = contour.close ? contour.len() : contour.len() - 1;

        for (usize i = 0; i < l; i++) {
            Math::Edgef curr = {contour[i], contour[(i + 1) % contour.len()]};

            if (curr.degenerated()) {
                continue;
            }

            auto outerCurr = curr.offset(outerDist);
            auto innerCurr = curr.offset(innerDist).swap();

            poly.pushBack(outerCurr);
            poly.pushBack(innerCurr);

            if (i == 0 and not contour.close) {
                auto center = (innerCurr.end + outerCurr.start) / 2;
                _createCap(poly, stroke, {innerCurr.end, outerCurr.start, center});
            }

            if (i + 1 == l and not contour.close) {
                auto center = (outerCurr.end + innerCurr.start) / 2;
                _createCap(poly, stroke, {outerCurr.end, innerCurr.start, center});
            }

            if (contour.close or i + 1 != l) {
                Math::Edgef next = {
                    contour[(i + 1) % contour.len()],
                    contour[(i + 2) % contour.len()],
                };

                // Make sure that the edge is not degenerate
                if (next.degenerated()) {
                    continue;
                }

                auto outerNext = outerDist > -0.001 ? next : next.offset(outerDist);
                auto innerNext = innerDist < 0.001 ? next.swap() : next.offset(innerDist).swap();

                if (outerDist < -0.001)
                    _createJoin(poly, stroke, outerCurr, outerNext, curr.end, Math::abs(outerDist));

                if (innerDist > 0.001)
                    _createJoin(poly, stroke, innerNext, innerCurr, curr.end, Math::abs(innerDist));
            }
        }
    }
}

void createSolid(Math::Polyf& poly, Math::Path const& path) {
    for (auto contour : path.iterContours()) {
        for (usize i = 0; i < contour.len(); i++) {
            Math::Edgef e = {contour[i], contour[(i + 1) % contour.len()]};
            poly.pushBack(e);
        }
    }
}

} // namespace Karm::Gfx
