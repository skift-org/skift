#pragma once

import Karm.Core;

#include "arc.h"
#include "curve.h"
#include "edge.h"
#include "ellipse.h"
#include "radii.h"
#include "rect.h"
#include "vec.h"

namespace Karm::Math {

struct Path {
    enum struct Code {
        NOP,
        CLEAR,
        CLOSE,
        MOVE_TO,
        LINE_TO,
        HLINE_TO,
        VLINE_TO,
        CUBIC_TO,
        QUAD_TO,
        ARC_TO,
    };

    using enum Code;

    enum struct Option {
        LARGE = 1 << 1,
        SWEEP = 1 << 2,
        SMOOTH = 1 << 3,
        RELATIVE = 1 << 4,
    };

    using enum Option;

    struct Op {
        Code code{};
        Flags<Option> options{};

        Math::Vec2f radii{};
        f64 angle{};
        Math::Vec2f cp1{};
        Math::Vec2f cp2{};
        Math::Vec2f p{};

        Op(Code code, Flags<Option> options = {})
            : code(code), options(options) {}

        Op(Code code, Math::Vec2f p, Flags<Option> options = {})
            : code(code), options(options), p(p) {}

        Op(Code code, Math::Vec2f cp2, Math::Vec2f p, Flags<Option> options = {})
            : code(code), options(options), cp2(cp2), p(p) {}

        Op(Code code, Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Flags<Option> options = {})
            : code(code), options(options), cp1(cp1), cp2(cp2), p(p) {}

        Op(Code code, Math::Vec2f radii, f64 angle, Math::Vec2f p, Flags<Option> options = {})
            : code(code), options(options), radii(radii), angle(angle), p(p) {}

        void repr(Io::Emit& e) const;
    };

    struct Contour {
        usize start{};
        usize end{};
        bool close{};
    };

    Vec<Contour> _contours{};
    Vec<Math::Vec2f> _verts{};

    Math::Vec2f _lastCp;
    Math::Vec2f _lastP;

    auto iterContours() const {
        struct _Contour : Slice<Math::Vec2f> {
            bool close;

            _Contour(Slice<Math::Vec2f> slice, bool close)
                : Slice<Math::Vec2f>(slice), close(close) {}
        };

        return Iter([&, i = 0uz] mutable -> Opt<_Contour> {
            if (i >= _contours.len()) {
                return NONE;
            }

            i++;

            return _Contour{
                sub(_verts, _contours[i - 1].start, _contours[i - 1].end),
                _contours[i - 1].close,
            };
        });
    }

    Opt<Math::Rectf> _bound;

    Math::Rectf bound();

    // MARK: Flattening --------------------------------------------------------

    void _flattenClose();

    void _flattenLineTo(Math::Vec2f p);

    void _flattenCurveTo(Math::Curvef c, isize depth = 0);

    void _flattenArcTo(Math::Vec2f start, Math::Vec2f radii, f64 angle, Flags<Option> options, Math::Vec2f point);

    // MARK: Operations --------------------------------------------------------

    void evalOp(Op op);

    void clear();

    void close();

    void moveTo(Math::Vec2f p, Flags<Option> options = {});

    void lineTo(Math::Vec2f p, Flags<Option> options = {});

    void hlineTo(f64 x, Flags<Option> options = {});

    void vlineTo(f64 y, Flags<Option> options = {});

    void cubicTo(Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Flags<Option> options = {});

    void smoothCubicTo(Math::Vec2f cp2, Math::Vec2f p, Flags<Option> options = {});

    void quadTo(Math::Vec2f cp, Math::Vec2f p, Flags<Option> options = {});

    void smoothQuadTo(Math::Vec2f p, Flags<Option> options = {});

    void arcTo(Math::Vec2f radii, f64 angle, Math::Vec2f p, Flags<Option> options = {});

    // MARK: Shapes ------------------------------------------------------------

    void line(Math::Edgef edge);

    void curve(Math::Curvef curve);

    void rect(Math::Rectf rect, Math::Radiif radii = 0);

    void ellipse(Math::Ellipsef ellipse);

    void arc(Math::Arcf arc);

    void path(Math::Path const& path);

    // MARK: Transform

    void offset(Math::Vec2f offset);

    // MARK: Svg ---------------------------------------------------------------

    Opt<Math::Vec2f> _nextVec2f(Io::SScan& scan);

    static Opt<Op> parseOp(Io::SScan& scan, Rune opcode);

    bool evalSvg(Str svg);

    static Path fromSvg(Str svg);

    // MARK: Format

    void repr(Io::Emit& e) const {
        e("(path");
        e.indentNewline();
        for (auto& contour : iterContours()) {
            e("(contour {})\n", contour);
        }
        e(")");
    }
};

} // namespace Karm::Math
