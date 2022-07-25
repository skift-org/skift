#pragma once

#include <karm-base/clamp.h>
#include <karm-base/try.h>
#include <karm-debug/logger.h>
#include <karm-gfx/path.h>

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
        if (!seg.close)
            continue;

        for (size_t i = 0; i < seg.len(); i++) {
            shape.add({seg[i], seg[(i + 1) % seg.len()]});
        }
    }
}

/*
static void createArc(Shape &shape, Math::Vec2f center, double startAngle, double endAngle, double radius) {
}
*/

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

/*
static void _createJoinRound(Shape &shape, Math::Edgef curr, Math::Edgef next, float width) {

}
*/

[[maybe_unused]] static void _createJoin(Shape &shape, Math::Edgef curr, Math::Edgef next, Math::Vec2f corner, float width, StrokeStyle::Join join) {
    /*if (Math::epsilonEq(curr.end, next.start, 0.001)) {
        return;
    }*/

    switch (join) {
    case StrokeStyle::Join::BEVEL:
        _createJoinBevel(shape, curr, next);
        break;
    case StrokeStyle::Join::MITER:
        _createJoinMiter(shape, curr, next, corner, width);
        break;
    /*case Stroke::Join::ROUND:
        _createJoinRound(shape, curr, next, width);
        break;*/
    default:
        panic("unknown join type");
    }
}

static void _createCapButt(Shape &shape, Math::Vec2f start, Math::Vec2f end) {
    shape.add({start, end});
}

static void _createCapSquare(Shape &shape, Math::Vec2f start, Math::Vec2f end, float width) {
    auto e = Math::Edgef{start, end}.parallel(width / 2);
    shape.add({start, e.start});
    shape.add(e);
    shape.add({e.end, end});
}

/*
static void _createCapRound(Shape &shape, Math::Vec2f start, Math::Vec2f end, float width) {
}
*/

[[maybe_unused]] static void _createCap(Shape &shape, Math::Vec2f start, Math::Vec2f end, float width, StrokeStyle::Cap cap) {
    switch (cap) {
    case StrokeStyle::Cap::BUTT:
        _createCapButt(shape, start, end);
        break;
    case StrokeStyle::Cap::SQUARE:
        _createCapSquare(shape, start, end, width);
        break;
    /*case StrokeStyle::Cap::ROUND:
        _createCapRound(shape, start, end, width);
        break;*/
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
        for (size_t i = 0; i < seg.len(); i++) {
            Math::Edgef curr = {seg[i], seg[(i + 1) % seg.len()]};

            auto outerCurr = curr.parallel(outerDist);
            auto innerCurr = curr.parallel(innerDist).swap();

            shape.add(outerCurr);
            shape.add(innerCurr);

            /*
            if (i == 0 && !seg.close) {
                _createCap(shape, innerCurr.start, outerCurr.start, stroke.width, stroke.cap);
            }

            if (i == seg.len() - 1 && !seg.close) {
                _createCap(shape, innerCurr.end, outerCurr.end, stroke.width, stroke.cap);
            }
*/

            if (seg.close) {
                Math::Edgef next = {
                    seg[(i + 1) % seg.len()],
                    seg[(i + 2) % seg.len()],
                };

                auto outerNext = next.parallel(outerDist);
                auto innerNext = next.parallel(innerDist).swap();

                if (outerDist < -0.001)
                    _createJoin(shape, outerCurr, outerNext, curr.end, stroke.width, stroke.join);
                if (innerDist > 0.001)
                    _createJoin(shape, innerNext, innerCurr, curr.end, stroke.width, stroke.join);
            }
        }
    }
}

} // namespace Karm::Gfx
