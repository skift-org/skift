#pragma once

#include <karm-base/enum.h>
#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-gfx/style.h>
#include <karm-io/expr.h>
#include <karm-io/sscan.h>
#include <karm-math/const.h>
#include <karm-math/edge.h>
#include <karm-math/ellipse.h>
#include <karm-math/funcs.h>
#include <karm-math/rect.h>
#include <karm-math/trans.h>
#include <karm-math/vec.h>

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

inline Str opCodeName(PathOpCode opCode) {
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

} // namespace _Impl

struct Path {
    using Code = _Impl::PathOpCode;
    using Flags = _Impl::PathOpFlags;

    using enum _Impl::PathOpCode;
    using enum _Impl::PathOpFlags;

    struct Op {
        Code code{};
        Flags flags{};

        Math::Vec2f radius{};
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

        Op(Code code, Math::Vec2f radius, f64 angle, Math::Vec2f p, Flags flags = DEFAULT)
            : code(code), flags(flags), radius(radius), angle(angle), p(p) {}
    };

    struct _Seg {
        usize start{};
        usize end{};
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
    Math::Trans2f _trans = Math::Trans2f::identity();

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

    void _flattenClose();

    void _flattenLineTo(Math::Vec2f p);

    void _flattenLineToNoTrans(Math::Vec2f p);

    void _flattenCubicTo(Math::Vec2f a, Math::Vec2f b, Math::Vec2f c, Math::Vec2f d);

    void _flattenCubicToNoTrans(Math::Vec2f a, Math::Vec2f b, Math::Vec2f c, Math::Vec2f d, isize depth);

    void _flattenQuadraticTo(Math::Vec2f start, Math::Vec2f cp, Math::Vec2f point);

    void _flattenArcTo(Math::Vec2f start, Math::Vec2f radius, f64 angle, Flags flags, Math::Vec2f point);

    /* --- Transform -------------------------------------------------------- */

    void transform(Math::Trans2f trans) {
        _trans = trans;
    }

    Math::Trans2f transform() {
        return _trans;
    }

    /* --- Operations ------------------------------------------------------- */

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

    void arcTo(Math::Vec2f radius, f64 angle, Math::Vec2f p, Flags flags = DEFAULT);

    /* --- Shapes ----------------------------------------------------------- */

    void line(Math::Edgef edge);

    void rect(Math::Rectf rect, BorderRadius radius = 0);

    void ellipse(Math::Ellipsef ellipse);

    /* --- Svg -------------------------------------------------------------- */

    Opt<Math::Vec2f> _nextVec2f(Io::SScan &scan);

    static Opt<Op> parseOp(Io::SScan &scan, Rune opcode);

    bool evalSvg(Str svg);

    static Path fromSvg(Str svg);
};

} // namespace Karm::Gfx
