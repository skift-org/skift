#pragma once

#include <karm-base/enum.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-io/expr.h>
#include <karm-io/sscan.h>

#include "arc.h"
#include "curve.h"
#include "edge.h"
#include "ellipse.h"
#include "radii.h"
#include "rect.h"
#include "vec.h"

namespace Karm::Math {

struct Path {
    enum Code {
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

    enum Flags {
        DEFAULT = 0,

        LARGE = 1 << 1,
        SWEEP = 1 << 2,
        SMOOTH = 1 << 3,
        RELATIVE = 1 << 4,
    };

    struct Op {
        Code code{};
        Flags flags{};

        Math::Vec2f radii{};
        f64 angle{};
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

        Op(Code code, Math::Vec2f radii, f64 angle, Math::Vec2f p, Flags flags = DEFAULT)
            : code(code), flags(flags), radii(radii), angle(angle), p(p) {}
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
        struct _Contour : public Slice<Math::Vec2f> {
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

    void _flattenArcTo(Math::Vec2f start, Math::Vec2f radii, f64 angle, Flags flags, Math::Vec2f point);

    // MARK: Operations --------------------------------------------------------

    void evalOp(Op op);

    void clear();

    void close();

    void moveTo(Math::Vec2f p, Flags flags = DEFAULT);

    void lineTo(Math::Vec2f p, Flags flags = DEFAULT);

    void hlineTo(f64 x, Flags flags = DEFAULT);

    void vlineTo(f64 y, Flags flags = DEFAULT);

    void cubicTo(Math::Vec2f cp1, Math::Vec2f cp2, Math::Vec2f p, Flags flags = DEFAULT);

    void smoothCubicTo(Math::Vec2f cp2, Math::Vec2f p, Flags flags = DEFAULT);

    void quadTo(Math::Vec2f cp, Math::Vec2f p, Flags flags = DEFAULT);

    void smoothQuadTo(Math::Vec2f p, Flags flags = DEFAULT);

    void arcTo(Math::Vec2f radii, f64 angle, Math::Vec2f p, Flags flags = DEFAULT);

    // MARK: Shapes ------------------------------------------------------------

    void line(Math::Edgef edge);

    void curve(Math::Curvef curve);

    void rect(Math::Rectf rect, Math::Radiif radii = 0);

    void ellipse(Math::Ellipsef ellipse);

    void arc(Math::Arcf arc);

    void path(Math::Path const& path);

    // MARK: Svg ---------------------------------------------------------------

    Opt<Math::Vec2f> _nextVec2f(Io::SScan& scan);

    static Opt<Op> parseOp(Io::SScan& scan, Rune opcode);

    bool evalSvg(Str svg);

    static Path fromSvg(Str svg);
};

FlagsEnum$(Path::Flags);

} // namespace Karm::Math
