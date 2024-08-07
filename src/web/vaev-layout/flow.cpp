#include "flow.h"

namespace Vaev::Layout {

void Flow::placeChildren(Context &ctx, Box box) {
    Frag::placeChildren(ctx, box);

    for (auto &c : _frags) {
        c->placeChildren(ctx, box);
    }
}

void Flow::makePaintables(Paint::Stack &stack) {
    Frag::makePaintables(stack);

    for (auto &c : _frags) {
        c->makePaintables(stack);
    }
}

void Flow::paintWireframe(Gfx::Canvas &g) {
    for (auto &c : _frags) {
        c->paintWireframe(g);
    }

    g.strokeStyle({
        .fill = Gfx::BLACK,
        .width = 1,
        .align = Gfx::INSIDE_ALIGN,
    });
    g.stroke(_box.borderBox.cast<f64>());
}

void Flow::repr(Io::Emit &e) const {
    e("({} {}", type(), _box.borderBox);
    if (_frags) {
        e.indentNewline();
        for (auto &c : _frags) {
            c->repr(e);
            e.newline();
        }
        e.deindent();
    }
    e(")");
}

} // namespace Vaev::Layout
