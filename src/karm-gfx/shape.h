#pragma once

#include <karm-base/clamp.h>
#include <karm-base/try.h>
#include <karm-gfx/path.h>

#include "path.h"
#include "style.h"

namespace Karm::Gfx {

struct Shape {
    Vec<Math::Edgef> _edges{};

    Math::Rectf bound() const {
        Opt<Math::Rectf> bound{};
        for (auto const &edge : *this) {
            if (bound) {
                bound = bound->mergeWith(edge.bound());
            } else {
                bound = edge.bound();
            }
        }
        return tryOr(bound, {});
    }

    Math::Edgef const &operator[](size_t i) const {
        return _edges[i];
    }

    Math::Edgef const *buf() const {
        return _edges.buf();
    }

    size_t len() const {
        return _edges.len();
    }

    Math::Edgef const *begin() const {
        return buf();
    }

    Math::Edgef const *end() const {
        return buf() + len();
    }

    void add(Math::Edgef const &edge) {
        if (edge.hasNan()) {
            panic("NaN in edge");
        }
        _edges.add(edge);
    }

    void clear() {
        _edges.clear();
    }
};

static void createSolid(Path &path, Shape &shape) {
    for (auto seg : path.iterSegs()) {
        for (size_t i = 0; i < seg.len(); i++) {
            shape.add({seg[i], seg[(i + 1) % seg.len()]});
        }
    }
}

static void _createArc(Shape &shape, Math::Vec2f center, double startAngle, double delta, double radius) {
    int devision = 32;
    double step = delta / devision;
    for (int i = 0; i < devision; i++) {
        double sa = startAngle + step * i;
        double ea = startAngle + step * (i + 1);
        shape.add({center + Math::Vec2f{radius * std::cos(sa), radius * std::sin(sa)},
                   center + Math::Vec2f{radius * std::cos(ea), radius * std::sin(ea)}});
    }
}

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
        return;
    }

    auto j = nextVec.cross(diffVec) / nextVec.cross(currVec);
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

[[maybe_unused]] static void _createJoin(Shape &shape, Math::Edgef curr, Math::Edgef next, Math::Vec2f corner, float width, float radius, StrokeStyle::Join join) {
    if (Math::epsilonEq(curr.end, next.start, 0.001)) {
        return;
    }

    switch (join) {
    case StrokeStyle::Join::BEVEL:
        _createJoinBevel(shape, curr, next);
        break;
    case StrokeStyle::Join::MITER:
        _createJoinMiter(shape, curr, next, corner, width);
        break;
    case StrokeStyle::Join::ROUND:
        _createJoinRound(shape, curr, next, corner, radius);
        break;
    default:
        panic("unknown join type");
    }
}

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

[[maybe_unused]] static void _createCap(Shape &shape, Math::Vec2f start, Math::Vec2f end, Math::Vec2f center, float width, StrokeStyle::Cap cap) {
    switch (cap) {
    case StrokeStyle::Cap::BUTT:
        _createCapButt(shape, start, end);
        break;
    case StrokeStyle::Cap::SQUARE:
        _createCapSquare(shape, start, end, width);
        break;
    case StrokeStyle::Cap::ROUND:
        _createCapRound(shape, start, end, center, width);
        break;
    default:
        panic("unknown cap type");
    }
}

static void createStroke(Path const &path, Shape &shape, StrokeStyle stroke) {
    double outerDist = 0;

    if (stroke.align == StrokeStyle::Align::CENTER) {
        outerDist = -stroke.width / 2;
    } else if (stroke.align == StrokeStyle::Align::OUTSIDE) {
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
                _createCap(shape, innerCurr.end, outerCurr.start, center, stroke.width, stroke.cap);
            }

            if (i + 1 == l && !seg.close) {
                auto center = (outerCurr.end + innerCurr.start) / 2;
                _createCap(shape, outerCurr.end, innerCurr.start, center, stroke.width, stroke.cap);
            }

            if (seg.close || i + 1 != l) {
                Math::Edgef next = {
                    seg[(i + 1) % seg.len()],
                    seg[(i + 2) % seg.len()],
                };

                auto outerNext = next.parallel(outerDist);
                auto innerNext = next.parallel(innerDist).swap();

                if (outerDist < -0.001)
                    _createJoin(shape, outerCurr, outerNext, curr.end, stroke.width, abs(outerDist), stroke.join);

                if (innerDist > 0.001)
                    _createJoin(shape, innerNext, innerCurr, curr.end, stroke.width, abs(innerDist), stroke.join);
            }
        }
    }
}

} // namespace Karm::Gfx
