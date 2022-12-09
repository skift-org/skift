#include <karm-debug/logger.h>

#include "path.h"

namespace Karm::Gfx {

/* --- Flattening ------------------------------------------------------- */

void Path::_flattenClose() {
    if (Math::epsilonEq(_lastP, _verts[last(_segs).start], 0.001)) {
        _verts.popBack();
        last(_segs).end--;
    }

    last(_segs).close = true;
}

void Path::_flattenLineTo(Math::Vec2f p) {
    if (_segs.len() == 0) {
        Debug::lerror("move to must be called before line to");
        return;
    }

    if (last(_segs).start != last(_segs).end &&
        Math::epsilonEq(_lastP, p, 0.001)) {
        return;
    }

    _verts.pushBack(p);
    last(_segs).end++;
}

void Path::_flattenCubicTo(Math::Vec2f a, Math::Vec2f b, Math::Vec2f c, Math::Vec2f d, int depth) {
    const int MAX_DEPTH = 16;
    const double TOLERANCE = 0.25;

    if (depth > MAX_DEPTH)
        return;

    auto d1 = d - a;
    auto d2 = Math::abs((b.x - d.x) * d1.y - (b.y - d.y) * d1.x);
    auto d3 = Math::abs((c.x - d.x) * d1.y - (c.y - d.y) * d1.x);

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

    _flattenCubicTo(a, ab, abc, abcd, depth + 1);
    _flattenCubicTo(abcd, bcd, cd, d, depth + 1);
}

void Path::_flattenQuadraticTo(Math::Vec2f start, Math::Vec2f cp, Math::Vec2f point) {
    auto cp1 = start + ((cp - start) * (2.0f / 3.0f));
    auto cp2 = point + ((cp - point) * (2.0f / 3.0f));
    _flattenCubicTo(start, cp1, cp2, point);
}

void Path::_flattenArcTo(Math::Vec2f start, Math::Vec2f radius, double angle, Flags flags, Math::Vec2f point) {
    // Ported from canvg (https://code.google.com/p/canvg/)
    double x1 = start.x;
    double y1 = start.y;
    double x2 = point.x;
    double y2 = point.y;

    double dx = x1 - x2;
    double dy = y1 - y2;
    double d = sqrtf(dx * dx + dy * dy);

    if (d < 1e-6f || radius.x < 1e-6f || radius.y < 1e-6f) {
        // The arc degenerates to a line
        _flattenLineTo(point);
        return;
    }

    double rotx = angle / 180.0f * M_PI; // x rotation angle
    double sinrx = sinf(rotx);
    double cosrx = cosf(rotx);

    // Convert to center point parameterization.
    // http://www.w3.org/TR/SVG11/implnote.html#ArcImplementationNotes

    // 1) Compute x1', y1'
    double x1p = cosrx * dx / 2.0f + sinrx * dy / 2.0f;
    double y1p = -sinrx * dx / 2.0f + cosrx * dy / 2.0f;

    d = Math::pow2(x1p) / Math::pow2(radius.x) + Math::pow2(y1p) / Math::pow2(radius.y);

    if (d > 1) {
        d = sqrtf(d);
        radius.x *= d;
        radius.y *= d;
    }

    // 2) Compute cx', cy'
    double sa = Math::pow2(radius.x) * Math::pow2(radius.y) - Math::pow2(radius.x) * Math::pow2(y1p) - Math::pow2(radius.y) * Math::pow2(x1p);
    double sb = Math::pow2(radius.x) * Math::pow2(y1p) + Math::pow2(radius.y) * Math::pow2(x1p);

    if (sa < 0.0f) {
        sa = 0.0f;
    }

    double s = 0.0f;

    if (sb > 0.0f) {
        s = sqrtf(sa / sb);
    }

    bool fa = !!(flags & LARGE);
    bool fs = !!(flags & SWEEP);

    if (fa == fs) {
        s = -s;
    }

    double cxp = s * radius.x * y1p / radius.y;
    double cyp = s * -radius.y * x1p / radius.x;

    // 3) Compute cx,cy from cx',cy'
    double cx = cosrx * cxp - sinrx * cyp + (x1 + x2) / 2.0f;
    double cy = sinrx * cxp + cosrx * cyp + (y1 + y2) / 2.0f;

    // 4) Calculate theta1, and delta theta.
    Math::Vec2f u = {(x1p - cxp) / radius.x, (y1p - cyp) / radius.y};
    Math::Vec2f v = {(-x1p - cxp) / radius.x, (-y1p - cyp) / radius.y};

    double a1 = Math::Vec2f(1, 0).angleWith(u); // Initial angle
    double da = u.angleWith(v);

    if (!fs && da > 0) {
        da -= 2 * M_PI;
    } else if (fs && da < 0) {
        da += 2 * M_PI;
    }

    // Approximate the arc using cubic spline segments.
    Math::Trans2f t{cosrx, sinrx, -sinrx, cosrx, cx, cy};

    // Split arc into max 90 degree segments.
    // The loop assumes an Iter per end point (including start and end), this +1.
    int ndivs = (int)(Math::abs(da) / (M_PI * 0.5f) + 1.0f);
    double hda = (da / (double)ndivs) / 2.0f;
    double kappa = Math::abs(4.0f / 3.0f * (1.0f - Math::cos(hda)) / Math::sin(hda));

    if (da < 0.0f) {
        kappa = -kappa;
    }

    Math::Vec2f current = {};
    Math::Vec2f ptan = {};

    for (int i = 0; i <= ndivs; i++) {
        double a = a1 + da * (i / (double)ndivs);

        dx = Math::cos(a);
        dy = Math::sin(a);

        Math::Vec2f p = t.apply(Math::Vec2f{dx * radius.x, dy * radius.y});
        Math::Vec2f tan = t.applyVector({-dy * radius.x * kappa, dx * radius.y * kappa});

        if (i > 0) {
            _flattenCubicTo(current, current + ptan, p - tan, p);
        }

        current = p;
        ptan = tan;
    }

    //_flattenLineTo(point);
}

/* --- Operations ------------------------------------------------------- */

void Path::evalOp(Op op) {
    if (_segs.len() > 0 &&
        last(_segs).close &&
        op.code != MOVE_TO &&
        op.code != CLEAR) {
        Debug::lerror("can't evalOp on closed path");
        return;
    }

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
        _flattenClose();
        break;

    case MOVE_TO:
        _segs.pushBack({_verts.len(), _verts.len(), false});
        _flattenLineTo(op.p);
        break;

    case LINE_TO:
        _flattenLineTo(op.p);
        break;

    case HLINE_TO:
        op.p = {op.p.x, _lastP.y};
        _flattenLineTo(op.p);
        break;

    case VLINE_TO:
        op.p = {_lastP.x, op.p.y};
        _flattenLineTo(op.p);
        break;

    case CUBIC_TO:
        if (op.flags & SMOOTH)
            op.cp1 = _lastP * 2 - _lastCp;

        _flattenCubicTo(_lastP, op.cp1, op.cp2, op.p);
        break;

    case QUAD_TO:
        if (op.flags & SMOOTH)
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

void Path::clear() {
    evalOp(CLEAR);
}

void Path::close() {
    evalOp(CLOSE);
}

void Path::moveTo(Math::Vec2f p, Flags flags) {
    evalOp({MOVE_TO, p, flags});
}

void Path::lineTo(Math::Vec2f p, Flags flags) {
    evalOp({LINE_TO, p, flags});
}

void Path::hlineTo(double x, Flags flags) {
    evalOp({HLINE_TO, {x, 0}, flags});
}

void Path::vlineTo(double y, Flags flags) {
    evalOp({VLINE_TO, {0, y}, flags});
}

void Path::cubicTo(Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Flags flags) {
    evalOp({CUBIC_TO, cp1, cp2, p, flags});
}

void Path::smoothCubicTo(Math::Vec2f cp2, Math::Vec2f p, Flags flags) {
    evalOp({CUBIC_TO, {}, cp2, p, flags | SMOOTH});
}

void Path::quadTo(Math::Vec2f cp, Math::Vec2f p, Flags flags) {
    evalOp({QUAD_TO, cp, p, flags});
}

void Path::smoothQuadTo(Math::Vec2f p, Flags flags) {
    evalOp({QUAD_TO, {}, p, flags | SMOOTH});
}

void Path::arcTo(Math::Vec2f radius, double angle, Math::Vec2f p, Flags flags) {
    evalOp({ARC_TO, radius, angle, p, flags});
}

/* --- Shapes ----------------------------------------------------------- */

void Path::line(Math::Edgef edge) {
    moveTo(edge.start);
    lineTo(edge.end);
}

void Path::rect(Math::Rectf rect, BorderRadius radius) {
    if (radius.zero()) {
        moveTo(rect.topStart());
        lineTo(rect.topEnd());
        lineTo(rect.bottomEnd());
        lineTo(rect.bottomStart());
        close();
    } else {
        auto maxRadius = min(rect.width, rect.height) / 2;

        radius.topLeft = min(radius.topLeft, maxRadius);
        radius.topRight = min(radius.topRight, maxRadius);
        radius.bottomRight = min(radius.bottomRight, maxRadius);
        radius.bottomLeft = min(radius.bottomLeft, maxRadius);

        double cpTopLeft = radius.topLeft - (radius.topLeft * 0.5522847498);
        double cpTopRight = radius.topRight - (radius.topRight * 0.5522847498);
        double cpBottomRight = radius.bottomRight - (radius.bottomRight * 0.5522847498);
        double cpBottomLeft = radius.bottomLeft - (radius.bottomLeft * 0.5522847498);

        moveTo({rect.x + radius.topLeft, rect.y});

        // Top edge
        lineTo({rect.x + rect.width - radius.topRight, rect.y});
        cubicTo({rect.x + rect.width - cpTopRight, rect.y},
                {rect.x + rect.width, rect.y + cpTopRight},
                {rect.x + rect.width, rect.y + radius.topRight});

        // Right edge
        lineTo({rect.x + rect.width, rect.y + rect.height - radius.bottomRight});
        cubicTo({rect.x + rect.width, rect.y + rect.height - cpBottomRight},
                {rect.x + rect.width - cpBottomRight, rect.y + rect.height},
                {rect.x + rect.width - radius.bottomRight, rect.y + rect.height});

        // Bottom edge
        lineTo({rect.x + radius.bottomLeft, rect.y + rect.height});
        cubicTo({rect.x + cpBottomLeft, rect.y + rect.height},
                {rect.x, rect.y + rect.height - cpBottomLeft},
                {rect.x, rect.y + rect.height - radius.bottomLeft});

        // Left edge
        lineTo({rect.x, rect.y + radius.topLeft});
        cubicTo({rect.x, rect.y + cpTopLeft},
                {rect.x + cpTopLeft, rect.y},
                {rect.x + radius.topLeft, rect.y});

        close();
    }
}

void Path::ellipse(Math::Ellipsef ellipse) {
    auto bound = ellipse.bound();
    moveTo(bound.topCenter());
    arcTo(ellipse.radius, 0, bound.bottomCenter(), Gfx::Path::SWEEP);
    arcTo(ellipse.radius, 0, bound.topCenter(), Gfx::Path::SWEEP);
    close();
}

/* --- Svg -------------------------------------------------------------- */

Opt<Math::Vec2f> _nextVec2f(Text::Scan &scan) {
    return Math::Vec2f{
        try$(scan.nextFloat()),
        try$(scan.nextFloat()),
    };
}

Opt<Path::Op> Path::parseOp(Text::Scan &scan, Rune opcode) {
    Flags flags{};
    flags |= islower(opcode) ? RELATIVE : DEFAULT;
    opcode = tolower(opcode);

    auto nextSep = [&]() {
        scan.skip(Re::optSeparator(','));
    };

    auto nextCoord = [&]() -> Opt<double> {
        nextSep();
        return try$(scan.nextFloat());
    };

    auto nextCoordPair = [&]() -> Opt<Math::Vec2f> {
        auto r = Math::Vec2f{try$(nextCoord()), try$(nextCoord())};
        nextSep();
        return r;
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

bool Path::evalSvg(Str svg) {
    Text::Scan scan{svg};

    Rune opcode{};
    while ((opcode = scan.next())) {
        scan.skip(Re::zeroOrMore(Re::space()));

        do {
            evalOp(try$(parseOp(scan, opcode)));
            scan.skip(Re::zeroOrMore(Re::space()));

            opcode = opcode == 'M' ? 'L' : opcode;
            opcode = opcode == 'm' ? 'l' : opcode;
        } while (!scan.ended() && !isalpha(scan.curr()));
    }

    return true;
}

Path Path::fromSvg(Str svg) {
    Path p;
    p.evalSvg(svg);
    return p;
}

} // namespace Karm::Gfx
