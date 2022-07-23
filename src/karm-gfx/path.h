#pragma once

#include <karm-base/enum.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-debug/logger.h>
#include <karm-math/edge.h>
#include <karm-math/ellipse.h>
#include <karm-math/funcs.h>
#include <karm-math/rect.h>
#include <karm-math/trans.h>
#include <karm-math/vec.h>
#include <karm-text/expr.h>
#include <karm-text/scan.h>

namespace Karm::Gfx {

namespace _Impl {

#define FOREACH_PATHOPCODE(OPCODE) \
    OPCODE(NOP)                    \
    OPCODE(CLEAR)                  \
    OPCODE(CLOSE)                  \
    OPCODE(MOVE_TO)                \
    OPCODE(LINE_TO)                \
    OPCODE(HLINE_TO)               \
    OPCODE(VLINE_TO)               \
    OPCODE(CUBIC_TO)               \
    OPCODE(QUAD_TO)                \
    OPCODE(ARC_TO)

enum PathOpCode {
#define ITER(OPCODE) OPCODE,
    FOREACH_PATHOPCODE(ITER)
#undef ITER
};

static inline Str opCodeName(PathOpCode opCode) {
    switch (opCode) {
#define ITER(OPCODE) \
    case OPCODE:     \
        return #OPCODE;
        FOREACH_PATHOPCODE(ITER)
#undef ITER
    }
    return "";
}

enum PathOpFlags {
    DEFAULT = 0,

    LARGE = 1 << 1,
    SWEEP = 1 << 2,
    SMOOTH = 1 << 3,
    RELATIVE = 1 << 4,
};

FlagsEnum$(PathOpFlags);

}; // namespace _Impl

struct Path {
    using Code = _Impl::PathOpCode;
    using Flags = _Impl::PathOpFlags;

    using enum _Impl::PathOpFlags;
    using enum _Impl::PathOpCode;

    struct Op {
        Code code{};
        Flags flags{};

        Math::Vec2f radius{};
        double angle{};
        Math::Vec2f cp1{};
        Math::Vec2f cp2{};
        Math::Vec2f p{};

        Op(Code code, Flags flags = DEFAULT)
            : code(code), flags(flags) {}

        Op(Code code, Math::Vec2f p, Flags flags = DEFAULT)
            : code(code), flags(flags), p(p) {}

        Op(Code code, Math::Vec2f cp2, Math::Vec2f p, Flags flags = DEFAULT)
            : code(code), flags(flags), cp2(cp2), p(p) {}

        Op(Code code, Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Flags flags = DEFAULT)
            : code(code), flags(flags), cp1(cp1), cp2(cp2), p(p) {}

        Op(Code code, Math::Vec2f radius, double angle, Math::Vec2f p, Flags flags = DEFAULT)
            : code(code), flags(flags), radius(radius), angle(angle), cp2(p), p(p) {}
    };

    struct _Seg {
        size_t start{};
        size_t end{};
        bool close{};
    };

    struct Seg : public Slice<Math::Vec2f> {
        bool close;
        Seg(Slice<Math::Vec2f> slice, bool close)
            : Slice<Math::Vec2f>(slice), close(close) {}
    };

    Vec<_Seg> _segs{};
    Vec<Math::Vec2f> _verts{};

    Math::Vec2f _lastCp;
    Math::Vec2f _lastP;

    auto iterSegs() const {
        return Iter([&, i = 0uz]() mutable -> Opt<Seg> {
            if (i >= _segs.len()) {
                return NONE;
            }

            i++;

            return Seg{
                sub(_verts, _segs[i - 1].start, _segs[i - 1].end),
                _segs[i - 1].close,
            };
        });
    }

    /* --- Flattening ------------------------------------------------------- */

    void _flattenLineTo(Math::Vec2f p) {
        _verts.pushBack(p);
        last(_segs).end++;
    }

    void _flattenCubicToImpl(Math::Vec2f a, Math::Vec2f b, Math::Vec2f c, Math::Vec2f d, int depth) {
        const int MAX_DEPTH = 16;
        const double TOLERANCE = 0.25;

        if (depth > MAX_DEPTH)
            return;

        auto d1 = d - a;
        auto d2 = abs((b.x - d.x) * d1.y - (b.y - d.y) * d1.x);
        auto d3 = abs((c.x - d.x) * d1.y - (c.y - d.y) * d1.x);

        if ((d2 + d3) * (d2 + d3) < TOLERANCE * (d1.x * d1.x + d1.y * d1.y)) {
            _flattenLineTo(d);
            return;
        }

        auto ab = (a + b) / 2;
        auto bc = (b + c) / 2;
        auto cd = (c + d) / 2;
        auto abc = (ab + bc) / 2;
        auto bcd = (bc + cd) / 2;
        auto abcd = (abc + bcd) / 2;

        _flattenCubicToImpl(a, ab, abc, abcd, depth + 1);
        _flattenCubicToImpl(abcd, bcd, cd, d, depth + 1);
    }

    void _flattenCubicTo(Math::Vec2f start, Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f point) {
        _flattenCubicToImpl(start, cp1, cp2, point, 0);
    }

    void _flattenQuadraticTo(Math::Vec2f start, Math::Vec2f cp, Math::Vec2f point) {
        auto cp1 = start + ((cp - start) * (2.0f / 3.0f));
        auto cp2 = point + ((cp - point) * (2.0f / 3.0f));
        _flattenCubicTo(start, cp1, cp2, point);
    }

    void _flattenArcTo(Math::Vec2f start, Math::Vec2f radius, float angle, Flags flags, Math::Vec2f point) {
        // Ported from canvg (https://code.google.com/p/canvg/)
        float x1 = start.x;
        float y1 = start.y;
        float x2 = point.x;
        float y2 = point.y;

        float dx = x1 - x2;
        float dy = y1 - y2;
        float d = sqrtf(dx * dx + dy * dy);

        if (d < 1e-6f || radius.x < 1e-6f || radius.y < 1e-6f) {
            // The arc degenerates to a line
            _flattenLineTo(point);
            return;
        }

        float rotx = angle / 180.0f * M_PI; // x rotation angle
        float sinrx = sinf(rotx);
        float cosrx = cosf(rotx);

        // Convert to center point parameterization.
        // http://www.w3.org/TR/SVG11/implnote.html#ArcImplementationNotes

        // 1) Compute x1', y1'
        float x1p = cosrx * dx / 2.0f + sinrx * dy / 2.0f;
        float y1p = -sinrx * dx / 2.0f + cosrx * dy / 2.0f;

        d = Math::pow2(x1p) / Math::pow2(radius.x) +
            Math::pow2(y1p) / Math::pow2(radius.y);

        if (d > 1) {
            d = sqrtf(d);
            radius = radius * d;
        }

        // 2) Compute cx', cy'
        float sa = Math::pow2(radius.x) * Math::pow2(radius.y) -
                   Math::pow2(radius.x) * Math::pow2(y1p) -
                   Math::pow2(radius.y) * Math::pow2(x1p);

        float sb = Math::pow2(radius.x) * Math::pow2(y1p) +
                   Math::pow2(radius.y) * Math::pow2(x1p);

        if (sa < 0.0f) {
            sa = 0.0f;
        }

        float s = 0.0f;

        if (sb > 0.0f) {
            s = sqrtf(sa / sb);
        }

        bool fa = !!(flags & LARGE);
        bool fs = !!(flags & SWEEP);

        if (fa == fs) {
            s = -s;
        }

        float cxp = s * radius.x * y1p / radius.y;
        float cyp = s * -radius.y * x1p / radius.x;

        // 3) Compute cx,cy from cx',cy'
        float cx = cosrx * cxp - sinrx * cyp + (x1 + x2) / 2.0f;
        float cy = sinrx * cxp + cosrx * cyp + (y1 + y2) / 2.0f;

        // 4) Calculate theta1, and delta theta.
        auto u = Math::Vec2f{(x1p - cxp) / radius.x, (y1p - cyp) / radius.y};
        auto v = Math::Vec2f{(-x1p - cxp) / radius.x, (-y1p - cyp) / radius.y};

        float a1 = Math::Vec2f{1, 0}.angleWith(u); // Initial angle
        float da = u.angleWith(v);

        if (!fs && da > 0) {
            da -= 2 * M_PI;
        } else if (fs && da < 0) {
            da += 2 * M_PI;
        }

        // Approximate the arc using cubic spline segments.
        Math::Trans2f t = {
            cosrx,
            sinrx,
            -sinrx,
            cosrx,
            cx,
            cy,
        };

        // Split arc into max 90 degree segments.
        // The loop assumes an Iter per end point (including start and end), this +1.
        int ndivs = (int)(fabsf(da) / (M_PI * 0.5f) + 1.0f);
        float hda = (da / (float)ndivs) / 2.0f;
        float kappa = fabsf(4.0f / 3.0f * (1.0f - cosf(hda)) / sinf(hda));

        if (da < 0.0f) {
            kappa = -kappa;
        }

        Math::Vec2f current = start;
        Math::Vec2f ptan{};

        for (int i = 0; i <= ndivs; i++) {
            float a = a1 + da * (i / (float)ndivs);

            dx = cosf(a);
            dy = sinf(a);

            auto p = t.applyPoint({dx * radius.x, dy * radius.y});
            auto tan = t.applyVector({-dy * radius.x * kappa, dx * radius.y * kappa});

            if (i > 0) {
                _flattenCubicTo(current, current + ptan, p - tan, p);
            }

            current = p;
            ptan = tan;
        }

        _flattenLineTo(point);
    }

    /* --- Operations ------------------------------------------------------- */

    void evalOp(Op op) {
        if (op.flags & RELATIVE) {
            op.cp1 = _lastP + op.cp1;
            op.cp2 = _lastP + op.cp2;
            op.p = _lastP + op.p;
            op.flags = op.flags & ~RELATIVE;
        }

        switch (op.code) {
        case CLEAR:
            _lastCp = {};
            _lastP = {};
            _segs.clear();
            _verts.clear();
            break;

        case CLOSE:
            _flattenLineTo(_verts[last(_segs).start]);
            last(_segs).close = true;
            break;

        case MOVE_TO:
            _segs.pushBack({_verts.len(), _verts.len(), false});
            _flattenLineTo(op.p);
            break;

        case LINE_TO:
            _flattenLineTo(op.p);
            break;

        case CUBIC_TO:
            if (op.flags & RELATIVE)
                op.cp1 = _lastP * 2 - _lastCp;

            _flattenCubicTo(_lastP, op.cp1, op.cp2, op.p);
            break;

        case QUAD_TO:
            if (op.flags & RELATIVE)
                op.cp2 = _lastP * 2 - _lastCp;

            _flattenQuadraticTo(_lastP, op.cp2, op.p);
            break;

        case ARC_TO:
            _flattenArcTo(_lastP, op.radius, op.angle, op.flags, op.p);
            break;

        default:
            panic("unknown path opcode");
        }

        _lastCp = op.cp2;
        _lastP = op.p;
    }

    /* --- Primitives ------------------------------------------------------- */

    void clear() {

        evalOp({CLEAR});
    }

    void close() {
        evalOp({CLOSE});
    }

    void moveTo(Math::Vec2f p, Flags flags = DEFAULT) {
        evalOp({MOVE_TO, p, flags});
    }

    void lineTo(Math::Vec2f p, Flags flags = DEFAULT) {
        evalOp({LINE_TO, p, flags});
    }

    void hlineTo(double x, Flags flags = DEFAULT) {
        evalOp({HLINE_TO, {x, 0}, flags});
    }

    void vlineTo(double y, Flags flags = DEFAULT) {
        evalOp({VLINE_TO, {0, y}, flags});
    }

    void cubicTo(Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Flags flags = DEFAULT) {
        evalOp({CUBIC_TO, cp1, cp2, p, flags});
    }

    void smoothCubicTo(Math::Vec2f cp2, Math::Vec2f p, Flags flags = DEFAULT) {
        evalOp({CUBIC_TO, {}, cp2, p, flags | SMOOTH});
    }

    void quadTo(Math::Vec2f cp, Math::Vec2f p, Flags flags = DEFAULT) {
        evalOp({QUAD_TO, cp, p, flags});
    }

    void smoothQuadTo(Math::Vec2f p, Flags flags = DEFAULT) {
        evalOp({QUAD_TO, {}, p, flags | SMOOTH});
    }

    void arcTo(Math::Vec2f radius, float angle, Math::Vec2f p, Flags flags = DEFAULT) {
        evalOp({ARC_TO, radius, angle, p, flags});
    }

    /* --- Shapes ----------------------------------------------------------- */

    void line(Math::Edgef edge) {
        moveTo(edge.start);
        lineTo(edge.end);
    }

    void rect(Math::Rectf rect) {
        moveTo(rect.topStart());
        lineTo(rect.topEnd());
        lineTo(rect.bottomEnd());
        lineTo(rect.bottomStart());
        close();
    }

    void ellipse(Math::Ellipsef ellipse) {
        auto bound = ellipse.bound();

        moveTo(bound.topCenter());
        arcTo(ellipse.radius, 0, bound.bottomCenter());
        arcTo(ellipse.radius, 0, bound.topCenter());
        close();
    }

    /* --- Svg -------------------------------------------------------------- */

    Opt<Math::Vec2f> _nextVec2f(Text::Scan &scan) {
        return Math::Vec2f{
            try$(scan.nextFloat()),
            try$(scan.nextFloat()),
        };
    }

    static Opt<Op> parseOp(Text::Scan &scan, Rune opcode) {
        Flags flags = DEFAULT;
        flags |= islower(opcode) ? RELATIVE : DEFAULT;
        opcode = tolower(opcode);

        auto nextSep = [&]() {
            scan.skip(Re::separator(','));
        };

        auto nextCoord = [&]() -> Opt<double> {
            nextSep();
            return try$(scan.nextFloat());
        };

        auto nextCoordPair = [&]() -> Opt<Math::Vec2f> {
            return Math::Vec2f{try$(nextCoord()), try$(nextCoord())};
        };

        switch (opcode) {
        case 'm': // move to
            return Op{MOVE_TO, try$(nextCoordPair()), flags};

        case 'z': // close
            return Op{CLOSE, flags};

        case 'l': // line to
            return Op{LINE_TO, try$(nextCoordPair()), flags};

        case 'h': // horizontal line to
            return Op{HLINE_TO, try$(nextCoord()), flags};

        case 'v': // vertical line to
            return Op{VLINE_TO, try$(nextCoord()), flags};

        case 'c': // cubic to
            return Op{CUBIC_TO, try$(nextCoordPair()), try$(nextCoordPair()), try$(nextCoordPair()), flags};

        case 's': // smooth cubic to
            return Op{CUBIC_TO, {}, try$(nextCoordPair()), try$(nextCoordPair()), flags | SMOOTH};

        case 'q': // quad to
            return Op{QUAD_TO, try$(nextCoordPair()), try$(nextCoordPair()), flags};

        case 't': // smooth quad to
            return Op{QUAD_TO, {}, try$(nextCoordPair()), flags | SMOOTH};

        case 'a': // arc to
        {
            auto radius = try$(nextCoordPair());
            auto angle = try$(nextCoord());

            nextSep();
            flags |= scan.next() == '1' ? LARGE : DEFAULT;

            nextSep();
            flags |= scan.next() == '1' ? SWEEP : DEFAULT;

            auto p = try$(nextCoordPair());
            return Op{ARC_TO, radius, angle, p, flags};
        }

        default:
            return NONE;
        }
    }

    bool evalSvg(Str svg) {
        Text::Scan scan{svg};

        Rune opcode{};
        while ((opcode = scan.next())) {

            do {
                evalOp(try$(parseOp(scan, opcode)));

                opcode = opcode == 'M' ? 'L' : opcode;
                opcode = opcode == 'm' ? 'l' : opcode;
            } while (!scan.ended() && !isalpha(scan.curr()));
        }

        return true;
    }

    static Path fromSvg(Str svg) {
        Path p;
        p.evalSvg(svg);
        return p;
    }
};

} // namespace Karm::Gfx
