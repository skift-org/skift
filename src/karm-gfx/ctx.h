#pragma once

#include <karm-base/vec.h>
#include <karm-math/rect.h>
#include <karm-text/str.h>

#include "buf.h"

namespace Karm::Gfx {

struct Ctx {
    struct _Ctx {
        Math::Recti clip;
        Math::Vec2i origin;
        Color fill;
    };

    Buf _buf;
    Base::Vec<_Ctx> _stack;

    /* --- Cycle ------------------------------------------------------------ */

    void begin(Buf buf);

    void end();

    void clear();

    /* --- Context ---------------------------------------------------------- */

    void push();

    void pop();

    void clip(Math::Recti rect);

    void origin(Math::Vec2i origin);

    void fill(Color color);

    /* --- Drawing ---------------------------------------------------------- */

    void dot(Math::Vec2i pos);

    void line(Math::Vec2i a, Math::Vec2i b);

    void rect(Math::Recti rect);

    void text(Math::Vec2i pos, Text::Str text);
};

} // namespace Karm::Gfx
