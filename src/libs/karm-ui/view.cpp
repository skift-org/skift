#include "view.h"

namespace Karm::Ui {

/* --- Text ----------------------------------------------------------------- */

struct Text : public View<Text> {
    Media::Font _font;
    String _text;
    Opt<Media::FontMesure> _mesure;

    Text(Media::Font font, String text)
        : _font(font), _text(text) {}

    void reconcile(Text &o) override {
        _text = o._text;
        _mesure = NONE;
    }

    Media::FontMesure mesure() {
        if (_mesure) {
            return *_mesure;
        }
        _mesure = _font.mesureStr(_text);
        return *_mesure;
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        auto m = mesure();
        auto baseline = bound().topStart() + m.baseline.cast<int>();

        g.textFont(_font);
        g.fill(baseline, _text);

        if (DEBUG) {
            g._line(
                {
                    bound().topStart() + m.baseline.cast<int>(),
                    bound().topEnd() + m.baseline.cast<int>(),
                },
                Gfx::PINK);
            g._rect(bound(), Gfx::CYAN);
        }
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return mesure().linebound.size().cast<int>();
    }
};

Strong<Media::Fontface> defaultFontface() {
    static Strong<Media::Fontface> f = []() {
        return Media::loadFontface("res/fonts/inter/Inter-Medium.ttf").unwrap();
    }();
    return f;
}

Child text(Media::Font font, Str text) {
    return makeStrong<Text>(font, text);
}

Child text(int size, Str text) {
    return makeStrong<Text>(Media::Font{(double)size, defaultFontface()}, text);
}

Child text(Str text) {
    return makeStrong<Text>(Media::Font{12, defaultFontface()}, text);
}

/* --- Icon ----------------------------------------------------------------- */

struct Icon : public View<Icon> {
    Media::Icon _icon;

    Icon(Media::Icon icon)
        : _icon(icon) {}

    void reconcile(Icon &o) override {
        _icon = o._icon;
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.fill(bound().topStart(), _icon);
        if (DEBUG)
            g._rect(bound(), Gfx::CYAN);
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return _icon.bound().size().cast<int>();
    }
};

Child icon(Media::Icon icon) {
    return makeStrong<Icon>(icon);
}

Child icon(Media::Icons i, double size) {
    return icon(Media::Icon{i, size});
}

} // namespace Karm::Ui
