#pragma once

#include "frag.h"

namespace Vaev::Layout {

struct Flow : public Frag {
    using Frag::Frag;

    Vec<Strong<Frag>> _frags;

    MutSlice<Strong<Frag>> children() override {
        return _frags;
    }

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

    void placeChildren(Context &ctx, Box box) override;

    void makePaintables(Paint::Stack &stack) override;

    void paintWireframe(Gfx::Canvas &g) override;

    void repr(Io::Emit &e) const override;
};

} // namespace Vaev::Layout
