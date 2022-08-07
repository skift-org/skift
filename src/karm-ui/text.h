#pragma once

#include "view.h"

namespace Karm::Ui {

struct Text : public View<Text> {
    Strong<Media::Font> _font;
    String _text;

    Text(Strong<Media::Font> font, String text)
        : _font(font), _text(text) {}

    void reconcile(Text &o) override {
        _text = o._text;
    }

    void paint(Gfx::Context &g) override {
        auto m = g.mesureStr(_text);
        g.fillStr(bound().topStart() + m.baseline.cast<int>(), _text);
    }

    Math::Vec2i size(Math::Vec2i) override {
        return _font->mesureStr(_text).linebound.size().cast<int>();
    }
};

static inline Child text(Str text) {
    return makeStrong<Text>(Media::Font::fallback(), text);
}

template <typename... Args>
static inline Child text(Str format, Args &&...args) {
    return makeStrong<Text>(Media::Font::fallback(), Fmt::format(format, std::forward<Args>(args)...));
}

static inline Child text(Strong<Media::Font> font, Str text) {
    return makeStrong<Text>(font, text);
}

template <typename... Args>
static inline Child text(Strong<Media::Font> font, Str format, Args &&...args) {
    return makeStrong<Text>(font, Fmt::format(format, std::forward<Args>(args)...));
}

} // namespace Karm::Ui
