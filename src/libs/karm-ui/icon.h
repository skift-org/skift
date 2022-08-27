#pragma once

#include <karm-media/icon.h>

#include "view.h"

namespace Karm::Ui {

struct Icon : public View<Icon> {
    Media::Icon _icon;

    Icon(Media::Icon icon)
        : _icon(icon) {}

    void reconcile(Icon &o) override {
        _icon = o._icon;
    }

    void paint(Gfx::Context &g) override {
        g.fill(bound().topStart(), _icon);
        if (DEBUG)
            g._rect(bound(), Gfx::CYAN);
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return _icon.bound().size().cast<int>();
    }
};

static inline Child icon(Media::Icon icon) {
    return makeStrong<Icon>(icon);
}

static inline Child icon(Media::Icons icon, double size = 18) {
    return makeStrong<Icon>(Media::Icon{icon, size});
}

} // namespace Karm::Ui
