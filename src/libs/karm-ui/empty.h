#pragma once

#include "view.h"

namespace Karm::Ui {

struct Empty : public View<Empty> {
    Math::Vec2i _size;

    Empty(Math::Vec2i size) : _size(size) {}

    void reconcile(Empty &o) override {
        _size = o._size;
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return _size;
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        if (DEBUG) {
            auto b = bound();
            g._rect(b, Gfx::WHITE.withOpacity(0.05));
            g._line({b.topStart(), b.bottomEnd()}, Gfx::WHITE.withOpacity(0.05));
            g._line({b.topEnd(), b.bottomStart()}, Gfx::WHITE.withOpacity(0.05));
        }
    }
};

static inline Child empty(Math::Vec2i size = {}) {
    return makeStrong<Empty>(size);
}

} // namespace Karm::Ui
