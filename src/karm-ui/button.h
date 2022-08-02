#pragma once

#include "karm-gfx/colors.h"
#include "proxy.h"
#include "text.h"

namespace Karm::Ui {

struct Button : public Proxy<Button> {
    Func<void()> _onPress;

    Button(Func<void()> onPress, Child child)
        : Proxy(child), _onPress(std::move(onPress)) {}

    void paint(Gfx::Context &g) const override {
        g.fillStyle(Gfx::BLUE500);
        g.fill(bound());
        child().paint(g);
    }
};

Child button(Func<void()> onPress, Child child) {
    return makeStrong<Button>(std::move(onPress), child);
}

Child button(Func<void()> onPress, Str t) {
    return button(std::move(onPress), text(t));
}

} // namespace Karm::Ui
