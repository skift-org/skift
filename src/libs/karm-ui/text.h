#pragma once

#include <karm-media/loader.h>

#include "view.h"

namespace Karm::Ui {

struct Text : public View<Text> {
    Media::Font _font;
    String _text;

    Text(Media::Font font, String text)
        : _font(font), _text(text) {}

    void reconcile(Text &o) override {
        _text = o._text;
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        auto m = _font.mesureStr(_text);
        auto baseline = bound().topStart() + m.baseline.cast<int>();

        g.textFont(_font);
        g.fill(baseline, _text);

        if (DEBUG) {
            g._line({
                        bound().topStart() + m.baseline.cast<int>(),
                        bound().topEnd() + m.baseline.cast<int>(),
                    },
                    Gfx::PINK);
            g._rect(bound(), Gfx::CYAN);
        }
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return _font.mesureStr(_text).linebound.size().cast<int>();
    }
};

static inline Child text(Str text) {
    return makeStrong<Text>(Media::loadFont(12, "res/fonts/inter/Inter-Medium.ttf").unwrap(), text);
}

template <typename... Args>
static inline Child text(Str format, Args &&...args) {
    return makeStrong<Text>(Media::loadFont(12, "res/fonts/inter/Inter-Medium.ttf").unwrap(), Fmt::format(format, std::forward<Args>(args)...));
}

static inline Child text(int size, Str text) {
    return makeStrong<Text>(Media::loadFont(size, "res/fonts/inter/Inter-Medium.ttf").unwrap(), text);
}

template <typename... Args>
static inline Child text(int size, Str format, Args &&...args) {
    return makeStrong<Text>(Media::loadFont(size, "res/fonts/inter/Inter-Medium.ttf").unwrap(), Fmt::format(format, std::forward<Args>(args)...));
}

static inline Child text(Media::Font font, Str text) {
    return makeStrong<Text>(font, text);
}

template <typename... Args>
static inline Child text(Media::Font font, Str format, Args &&...args) {
    return makeStrong<Text>(font, Fmt::format(format, std::forward<Args>(args)...));
}

} // namespace Karm::Ui
