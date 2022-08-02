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

static inline Child text(Str text) {
    return makeStrong<Text>(text);
}

template <typename... Args>
static inline Child text(Str format, Args &&...args) {
    return makeStrong<Text>(Fmt::format(format, std::forward<Args>(args)...));
}

} // namespace Karm::Ui
