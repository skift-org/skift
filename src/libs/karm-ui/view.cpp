#include "view.h"

#include "proxy.h"

namespace Karm::Ui {

/* --- Text ----------------------------------------------------------------- */

Strong<Media::Fontface> regularFontface() {
    static Strong<Media::Fontface> f = []() {
        return Media::loadFontface("res/fonts/inter/Inter-Medium.ttf").unwrap();
    }();
    return f;
}

Strong<Media::Fontface> boldFontface() {
    static Strong<Media::Fontface> f = []() {
        return Media::loadFontface("res/fonts/inter/Inter-Bold.ttf").unwrap();
    }();
    return f;
}

Strong<Media::Fontface> italicFontface() {
    static Strong<Media::Fontface> f = []() {
        return Media::loadFontface("res/fonts/inter/Inter-Italic.ttf").unwrap();
    }();
    return f;
}

TextStyle TextStyle::regular() {
    return {
        .font = Media::Font{
            12,
            regularFontface(),
        },
    };
}

TextStyle TextStyle::bold() {
    return {
        .font = Media::Font{
            12,
            boldFontface(),
        },
    };
}

TextStyle TextStyle::italic() {
    return {
        .font = Media::Font{
            12,
            italicFontface(),
        },
    };
}

TextStyle TextStyle::title1() {
    return {
        .font = Media::Font{
            32,
            boldFontface(),
        },
    };
}

TextStyle TextStyle::title2() {
    return {
        .font = Media::Font{
            24,
            boldFontface(),
        },
    };
}

TextStyle TextStyle::title3() {
    return {
        .font = Media::Font{
            16,
            boldFontface(),
        },
    };
}
TextStyle TextStyle::subtitle1() {
    return {
        .font = Media::Font{
            16,
            regularFontface(),
        },
    };
}
TextStyle TextStyle::subtitle2() {
    return {
        .font = Media::Font{
            14,
            regularFontface(),
        },
    };
}
TextStyle TextStyle::label() {
    return {
        .font = Media::Font{
            12,
            regularFontface(),
        },
    };
}
TextStyle TextStyle::body() {
    return {
        .font = Media::Font{
            12,
            regularFontface(),
        },
    };
}

struct Text : public View<Text> {
    TextStyle _style;
    String _text;
    Opt<Media::FontMesure> _mesure;

    Text(TextStyle style, String text)
        : _style(style), _text(text) {}

    void reconcile(Text &o) override {
        _text = o._text;
        _mesure = NONE;
    }

    Media::FontMesure mesure() {
        if (_mesure) {
            return *_mesure;
        }
        _mesure = _style.font.mesureStr(_text);
        return *_mesure;
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();

        auto m = mesure();
        auto baseline = bound().topStart() + m.baseline.cast<int>();

        if (_style.color) {
            g.fillStyle(*_style.color);
        }

        g.textFont(_style.font);
        g.fill(baseline, _text);

        if (debugShowLayoutBounds) {
            g._line(
                {
                    bound().topStart() + m.baseline.cast<int>(),
                    bound().topEnd() + m.baseline.cast<int>(),
                },
                Gfx::PINK);
            g._rect(bound(), Gfx::CYAN);
        }

        g.restore();
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return mesure().linebound.size().cast<int>();
    }
};

Child text(TextStyle style, Str text) {
    return makeStrong<Text>(style, text);
}

Child text(Str text) {
    return makeStrong<Text>(TextStyle::regular(), text);
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
        if (debugShowLayoutBounds)
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

/* --- Image ---------------------------------------------------------------- */

struct Image : public View<Image> {
    Media::Image image;

    Image(Media::Image image)
        : image(image) {
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.blit(bound(), image);
        if (debugShowLayoutBounds)
            g._rect(bound(), Gfx::CYAN);
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return image.bound().size().cast<int>();
    }
};

Child image(Media::Image image) {
    return makeStrong<Image>(image);
}

/* --- Canvas --------------------------------------------------------------- */

struct Canvas : public View<Canvas> {
    OnPaint _onPaint;

    Canvas(OnPaint onPaint)
        : _onPaint(std::move(onPaint)) {}

    void reconcile(Canvas &o) override {
        _onPaint = std::move(o._onPaint);
        View<Canvas>::reconcile(o);
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();
        g.clip(_bound);
        g.origin(_bound.xy);
        _onPaint(g, _bound.wh);
        g.restore();
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint hint) override {
        if (hint == Layout::Hint::MIN) {
            return 0;
        } else {
            return _bound.wh;
        }
    }
};

Child canvas(OnPaint onPaint) {
    return makeStrong<Canvas>(std::move(onPaint));
}

/* --- Blur ----------------------------------------------------------------- */

struct Blur : public Proxy<Blur> {
    int _radius;

    Blur(int radius, Child child)
        : Proxy<Blur>(std::move(child)), _radius(radius) {}

    void reconcile(Blur &o) override {
        _radius = o._radius;
        Proxy<Blur>::reconcile(o);
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.blur(bound(), _radius);
        Proxy<Blur>::paint(g, r);
    }
};

Child blur(int radius, Child child) {
    return makeStrong<Blur>(radius, std::move(child));
}

} // namespace Karm::Ui
