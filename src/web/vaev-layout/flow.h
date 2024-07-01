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

    void layout(RectPx bound) override {
        Frag::layout(bound);
        for (auto &c : _frags) {
            c->layout(bound);
        }
    }

    void paint(Paint::Stack &stack) override {
        Frag::paint(stack);

        for (auto &c : _frags) {
            c->paint(stack);
        }
    }

    void debug(Gfx::Context &g) override {
        for (auto &c : _frags) {
            c->debug(g);
        }

        g.plot(_borderBox.cast<isize>(), Gfx::RED);
    }

    void repr(Io::Emit &e) const override {
        e("({}", type());
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
