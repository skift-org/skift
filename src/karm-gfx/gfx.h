#pragma once

#include <karm-base/string.h>
#include <karm-base/vec.h>
#include <karm-math/rect.h>
#include <karm-math/trans.h>

#include "buf.h"
#include "color.h"

namespace Karm::Gfx {

struct Paint {
};

struct FillStyle {
    Paint paint;
};

struct StrokeStyle {
    Paint paint;
};

struct TextStyle {
    Paint paint;
};

struct Ctx {
    struct _Scope {
        Math::Recti clip;
        Math::Vec2i origin;
        Math::Trans2 trans;

        FillStyle fill;
        StrokeStyle stroke;
        TextStyle text;
    };

    Buf _buf;
    Vec<_Scope> _stack;

    /* --- Cycle ------------------------------------------------------------ */

    void begin(Buf buf);

    void end();

    void clear();

    /* --- Context ---------------------------------------------------------- */

    void push();

    void pop();

    void scope(auto inner) {
        push();
        inner();
        pop();
    }

    void clip(Math::Recti rect);

    void origin(Math::Vec2i origin);

    void trans(Math::Trans2 trans);

    void fillStyle(FillStyle style);

    void strokeStyle(StrokeStyle style);

    void textStyle(TextStyle style);

    /* --- Path ------------------------------------------------------------- */

    void beginPath();

    void closePath();

    void fill(Color color);

    void stroke();

    void moveTo(Math::Vec2i pos);

    void lineTo(Math::Vec2i pos);

    void quadraticTo(Math::Vec2i ctrl, Math::Vec2i pos);

    void bezierTo(Math::Vec2i ctrl1, Math::Vec2i ctrl2, Math::Vec2i pos);

    void arcTo(float rx, float ry, float angle, bool largeArc, bool sweep, Math::Vec2i pos);

    /* --- Drawing ---------------------------------------------------------- */

    void plotDot(Math::Vec2i pos);

    void strokeLine(Math::Vec2i a, Math::Vec2i b);

    void fillRect(Math::Recti rect);

    void strokeRect(Math::Recti rect);

    void fillCircle(Math::Vec2i pos, float radius);

    void strokeCircle(Math::Vec2i pos, float radius);

    void fillText(Math::Vec2i pos, Str text);

    void strokeText(Math::Vec2i pos, Str text);
};

} // namespace Karm::Gfx
