#pragma once

#include "frag.h"

namespace Vaev::Layout {

struct Flow : public Frag {
    using Frag::Frag;

    Vec<Strong<Frag>> _frags;

    Frag &fragAt(usize frag) {
        return *_frags[frag];
    }

    Frag const &fragAt(usize frag) const {
        return *_frags[frag];
    }

    Style::Computed const &styleAt(usize frag) const {
        return _frags[frag]->style();
    }

    void add(Strong<Frag> frag) {
        _frags.pushBack(frag);
    }

    void placeChildren(Context &ctx, Box box) override {
        Frag::placeChildren(ctx, box);

        for (auto &c : _frags) {
            c->placeChildren(ctx, box);
        }
    }

    void makePaintables(Paint::Stack &stack) override {
        Frag::makePaintables(stack);

        for (auto &c : _frags) {
            c->makePaintables(stack);
        }
    }

    void paintWireframe(Gfx::Context &g) override {
        for (auto &c : _frags) {
            c->paintWireframe(g);
        }

        g.strokeStyle({
            .paint = Gfx::BLACK,
            .width = 1,
            .align = Gfx::INSIDE_ALIGN,
        });
        g.stroke(_box.borderBox.cast<f64>());
    }

    void repr(Io::Emit &e) const override {
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
};

} // namespace Vaev::Layout
