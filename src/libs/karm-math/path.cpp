#include <karm-io/aton.h>
#include <karm-logger/logger.h>

#include "path.h"
#include "trans.h"

namespace Karm::Math {

Math::Rectf Path::bound() {
    if (isEmpty(_verts))
        return {};

    if (_bound)
        return *_bound;

    Math::Rectf rect = {_verts[0], 0};
    for (auto& p : _verts)
        rect = rect.mergeWith(p);
    _bound = rect;
    return rect;
}

// MARK: Flattening ------------------------------------------------------------

void Path::_flattenClose() {
    auto end = _verts[last(_contours).end - 1];
    auto start = _verts[last(_contours).start];

    if (Math::epsilonEq(start, end, 0.001)) {
        _verts.popBack();
        last(_contours).end--;
    }

    last(_contours).close = true;
}

void Path::_flattenLineTo(Math::Vec2f p) {
    if (not _contours.len()) {
        logError("path: moveTo must be called before lineTo");
        return;
    }

    if (last(_contours).start != last(_contours).end and
        Math::epsilonEq(last(_verts), p)) {
        return;
    }

    _verts.pushBack(p);
    last(_contours).end++;
}

void Path::_flattenCurveTo(Math::Curvef curve, isize depth) {
    if (depth > 16)
        return;

    if (curve.degenerated())
        return;

    if (curve.straightish(0.25)) {
        _flattenLineTo(curve.d);
        return;
    }

    auto [left, right] = curve.split();
    _flattenCurveTo(left, depth + 1);
    _flattenCurveTo(right, depth + 1);
}

void Path::_flattenArcTo(Math::Vec2f start, Math::Vec2f radii, f64 angle, Flags flags, Math::Vec2f point) {
    // Ported from canvg (https://github.com/canvg/canvg)
    f64 x1 = start.x;
    f64 y1 = start.y;
    f64 x2 = point.x;
    f64 y2 = point.y;

    f64 dx = x1 - x2;
    f64 dy = y1 - y2;
    f64 d = Math::sqrt(dx * dx + dy * dy);

    if (d < 1e-6f or radii.x < 1e-6f or radii.y < 1e-6f) {
        // The arc degenerates to a line
        _flattenLineTo(point);
        return;
    }

    f64 rotx = angle / 180.0 * Math::PI; // x rotation angle
    f64 sinrx = Math::sin(rotx);
    f64 cosrx = Math::cos(rotx);

    // Convert to center point parameterization.
    // http://www.w3.org/TR/SVG11/implnote.html#ArcImplementationNotes

    // 1) Compute x1', y1'
    f64 x1p = cosrx * dx / 2.0 + sinrx * dy / 2.0;
    f64 y1p = -sinrx * dx / 2.0 + cosrx * dy / 2.0;

    d = Math::pow2(x1p) / Math::pow2(radii.x) + Math::pow2(y1p) / Math::pow2(radii.y);

    if (d > 1) {
        d = Math::sqrt(d);
        radii.x *= d;
        radii.y *= d;
    }

    // 2) Compute cx', cy'
    f64 sa = Math::pow2(radii.x) * Math::pow2(radii.y) -
             Math::pow2(radii.x) * Math::pow2(y1p) -
             Math::pow2(radii.y) * Math::pow2(x1p);

    f64 sb = Math::pow2(radii.x) * Math::pow2(y1p) +
             Math::pow2(radii.y) * Math::pow2(x1p);

    if (sa < 0.0)
        sa = 0.0;

    f64 s = 0.0;

    if (sb > 0.0)
        s = Math::sqrt(sa / sb);

    bool fa = flags & LARGE;
    bool fs = flags & SWEEP;

    if (fa == fs) {
        s = -s;
    }

    f64 cxp = s * radii.x * y1p / radii.y;
    f64 cyp = s * -radii.y * x1p / radii.x;

    // 3) Compute cx,cy from cx',cy'
    f64 cx = cosrx * cxp - sinrx * cyp + (x1 + x2) / 2.0;
    f64 cy = sinrx * cxp + cosrx * cyp + (y1 + y2) / 2.0;

    // 4) Calculate theta1, and delta theta.
    Math::Vec2f u = {(x1p - cxp) / radii.x, (y1p - cyp) / radii.y};
    Math::Vec2f v = {(-x1p - cxp) / radii.x, (-y1p - cyp) / radii.y};

    f64 a1 = Math::Vec2f(1, 0).angleWith(u); // Initial angle
    f64 da = u.angleWith(v);

    if (not fs and da > 0) {
        da -= 2 * Math::PI;
    } else if (fs and da < 0) {
        da += 2 * Math::PI;
    }

    // Approximate the arc using cubic spline segments.
    Math::Trans2f t{cosrx, sinrx, -sinrx, cosrx, cx, cy};

    // Split arc into max 90 degree segments.
    // The loop assumes an Iter per end point (including start and end), this +1.
    isize ndivs = (isize)(Math::abs(da) / (Math::PI * 0.5) + 1.0);
    f64 hda = (da / (f64)ndivs) / 2.0;
    f64 kappa = Math::abs(4.0 / 3.0 * (1.0 - Math::cos(hda)) / Math::sin(hda));

    if (da < 0.0) {
        kappa = -kappa;
    }

    Math::Vec2f current = {};
    Math::Vec2f ptan = {};

    for (isize i = 0; i <= ndivs; i++) {
        f64 a = a1 + da * (i / (f64)ndivs);

        dx = Math::cos(a);
        dy = Math::sin(a);

        Math::Vec2f p = t.apply(Math::Vec2f{dx * radii.x, dy * radii.y});
        Math::Vec2f tan = t.applyVector({-dy * radii.x * kappa, dx * radii.y * kappa});

        if (i > 0)
            _flattenCurveTo(Math::Curvef::cubic(current, current + ptan, p - tan, p));

        current = p;
        ptan = tan;
    }
}

// MARK: Operations --------------------------------------------------------

void Path::evalOp(Op op) {
    if (_contours.len() > 0 and
        last(_contours).close and
        op.code != MOVE_TO and
        op.code != CLEAR) {
        logError("path: can't evalOp on closed path");
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
        _contours.clear();
        _verts.clear();
        break;

    case CLOSE:
        op.p = _verts[last(_contours).start];
        _flattenClose();
        break;

    case MOVE_TO:
        _contours.pushBack({_verts.len(), _verts.len(), false});
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
        _flattenCurveTo(Math::Curvef::cubic(_lastP, op.cp1, op.cp2, op.p));
        break;

    case QUAD_TO:
        if (op.flags & SMOOTH)
            op.cp2 = _lastP * 2 - _lastCp;
        _flattenCurveTo(Math::Curvef::quadratic(_lastP, op.cp2, op.p));
        break;

    case ARC_TO:
        _flattenArcTo(_lastP, op.radii, op.angle, op.flags, op.p);
        break;

    default:
        panic("unknown path opcode");
    }

    _lastCp = op.cp2;
    _lastP = op.p;
}

// MARK: Primitives --------------------------------------------------------

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

void Path::hlineTo(f64 x, Flags flags) {
    evalOp({HLINE_TO, {x, 0}, flags});
}

void Path::vlineTo(f64 y, Flags flags) {
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

void Path::arcTo(Math::Vec2f radii, f64 angle, Math::Vec2f p, Flags flags) {
    evalOp({ARC_TO, radii, angle, p, flags});
}

// MARK: Shapes ----------------------------------------------------------------

void Path::line(Math::Edgef edge) {
    moveTo(edge.start);
    lineTo(edge.end);
}

void Path::curve(Math::Curvef curve) {
    moveTo(curve.a);
    cubicTo(curve.b, curve.c, curve.d);
}

void Path::rect(Math::Rectf rect, Math::Radiif radii) {
    if (Math::epsilonEq(min(rect.width, rect.height), 0.0, 0.001))
        return;

    if (radii.zero()) {
        moveTo(rect.topStart());
        lineTo(rect.topEnd());
        lineTo(rect.bottomEnd());
        lineTo(rect.bottomStart());
        close();
    } else {
        radii = radii.reduceOverlap(rect.size());

        // NOTE: 0.5522847498 is the cubic bezier approximation of the circle
        //       Since we take the value relative to the end of the edge,
        //       we need to subtract it from the radii to get the control point
        f64 cpa = radii.a * (1 - 0.5522847498);
        f64 cpb = radii.b * (1 - 0.5522847498);
        f64 cpc = radii.c * (1 - 0.5522847498);
        f64 cpd = radii.d * (1 - 0.5522847498);
        f64 cpe = radii.e * (1 - 0.5522847498);
        f64 cpf = radii.f * (1 - 0.5522847498);
        f64 cpg = radii.g * (1 - 0.5522847498);
        f64 cph = radii.h * (1 - 0.5522847498);

        moveTo({rect.x + radii.b, rect.y});

        // Top end edge
        lineTo({rect.x + rect.width - radii.c, rect.y});
        cubicTo(
            {rect.x + rect.width - cpc, rect.y},
            {rect.x + rect.width, rect.y + cpd},
            {rect.x + rect.width, rect.y + radii.d}
        );

        // Bottom end edge
        lineTo({rect.x + rect.width, rect.y + rect.height - radii.e});
        cubicTo(
            {rect.x + rect.width, rect.y + rect.height - cpe},
            {rect.x + rect.width - cpf, rect.y + rect.height},
            {rect.x + rect.width - radii.f, rect.y + rect.height}
        );

        // Bottom start edge
        lineTo({rect.x + radii.g, rect.y + rect.height});
        cubicTo(
            {rect.x + cpg, rect.y + rect.height},
            {rect.x, rect.y + rect.height - cph},
            {rect.x, rect.y + rect.height - radii.h}
        );

        // Top start edge
        lineTo({rect.x, rect.y + radii.a});
        cubicTo(
            {rect.x, rect.y + cpa},
            {rect.x + cpb, rect.y},
            {rect.x + radii.b, rect.y}
        );

        close();
    }
}

void Path::ellipse(Math::Ellipsef ellipse) {
    auto bound = ellipse.bound();
    moveTo(bound.topCenter());
    arcTo(ellipse.radii, 0, bound.bottomCenter(), Path::SWEEP);
    arcTo(ellipse.radii, 0, bound.topCenter(), Path::SWEEP);
    close();
}

void Path::arc(Math::Arcf arc) {
    moveTo(arc.eval(0.0));
    for (auto t = 0.0; t < 1.0; t += 0.1) {
        auto p = arc.eval(t);
        lineTo(p);
    }
}

void Path::path(Math::Path const& path) {
    for (auto contour : path.iterContours()) {
        moveTo(first(contour));
        for (auto v : next(contour, 1)) {
            lineTo(v);
        }

        if (contour.close)
            close();
    }
}

// MARK: Transform

void Path::offset(Math::Vec2f offset) {
    for (auto& v : _verts)
        v = v + offset;
}

// MARK: Svg -------------------------------------------------------------------

Opt<Math::Vec2f> _nextVec2f(Io::SScan& s) {
    return Math::Vec2f{
        try$(Io::atof(s)),
        try$(Io::atof(s)),
    };
}

Opt<Path::Op> Path::parseOp(Io::SScan& s, Rune opcode) {
    Flags flags{};
    flags |= isAsciiLower(opcode) ? RELATIVE : DEFAULT;
    opcode = toAsciiLower(opcode);

    auto nextSep = [&] {
        s.skip(Re::optSeparator(','_re));
    };

    auto nextCoord = [&] -> Opt<f64> {
        nextSep();
        return try$(Io::atof(s));
    };

    auto nextCoordPair = [&] -> Opt<Math::Vec2f> {
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
        auto radii = try$(nextCoordPair());
        auto angle = try$(nextCoord());

        nextSep();
        flags |= s.next() == '1' ? LARGE : DEFAULT;

        nextSep();
        flags |= s.next() == '1' ? SWEEP : DEFAULT;

        auto p = try$(nextCoordPair());
        return Op{ARC_TO, radii, angle, p, flags};
    }

    default:
        return NONE;
    }
}

bool Path::evalSvg(Str svg) {
    Io::SScan s{svg};

    Rune opcode{};
    while ((opcode = s.next())) {
        s.skip(Re::zeroOrMore(Re::space()));

        do {
            auto maybeOp = parseOp(s, opcode);

            if (not maybeOp) {
                return false;
            }

            evalOp(maybeOp.unwrap());
            s.skip(Re::zeroOrMore(Re::space()));

            opcode = opcode == 'M' ? 'L' : opcode;
            opcode = opcode == 'm' ? 'l' : opcode;
        } while (not s.ended() and not isAsciiAlpha(s.peek()));
    }

    return true;
}

Path Path::fromSvg(Str svg) {
    Path p;
    if (not p.evalSvg(svg)) {
        logWarn("couldn't create SVG from input");
    }
    return p;
}

} // namespace Karm::Math
