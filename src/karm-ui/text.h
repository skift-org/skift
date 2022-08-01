#pragma once

#include "view.h"

namespace Karm::Ui {

struct Text : public View<Text> {
    String _text;

    Text(String text) : _text(text) {}

    void reconcile(Text &o) override {
        _text = o._text;
    }

    void paint(Gfx::Context &g) const override {
        g.fill({0, 0}, _text);
    }
};

template <typename... Args>
Child text(Args &&...args) {
    return makeStrong<Text>(std::forward<Args>(args)...);
}

} // namespace Karm::Ui
