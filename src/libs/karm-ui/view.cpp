#include "view.h"

#include "box.h"

namespace Karm::Ui {

/* --- Text ----------------------------------------------------------------- */

static Opt<Strong<Media::Fontface>> _regularFontface = NONE;
Strong<Media::Fontface> regularFontface() {
    if (!_regularFontface) {
        _regularFontface = Media::loadFontfaceOrFallback("bundle://inter-font/fonts/Inter-Regular.ttf"_url).unwrap();
    }
    return *_regularFontface;
}

static Opt<Strong<Media::Fontface>> _mediumFontface = NONE;
Strong<Media::Fontface> mediumFontface() {
    if (!_mediumFontface) {
        _mediumFontface = Media::loadFontfaceOrFallback("bundle://inter-font/fonts/Inter-Medium.ttf"_url).unwrap();
    }
    return *_mediumFontface;
}

static Opt<Strong<Media::Fontface>> _boldFontface = NONE;
Strong<Media::Fontface> boldFontface() {
    if (!_boldFontface) {
        _boldFontface = Media::loadFontfaceOrFallback("bundle://inter-font/fonts/Inter-Bold.ttf"_url).unwrap();
    }
    return *_boldFontface;
}

static Opt<Strong<Media::Fontface>> _italicFontface = NONE;
Strong<Media::Fontface> italicFontface() {
    if (!_italicFontface) {
        _italicFontface = Media::loadFontfaceOrFallback("bundle://inter-font/fonts/Inter-Italic.ttf"_url).unwrap();
    }
    return *_italicFontface;
}

TextStyle TextStyle::displayLarge() {
    return {
        .font = Media::Font{
            regularFontface(),
            57,
        },
    };
}

TextStyle TextStyle::displayMedium() {
    return {
        .font = Media::Font{
            regularFontface(),
            45,
        },
    };
}

TextStyle TextStyle::displaySmall() {
    return {
        .font = Media::Font{
            regularFontface(),
            36,
        },
    };
}

TextStyle TextStyle::headlineLarge() {
    return {
        .font = Media::Font{
            regularFontface(),
            32,
        },
    };
}

TextStyle TextStyle::headlineMedium() {
    return {
        .font = Media::Font{
            regularFontface(),
            28,
        },
    };
}

TextStyle TextStyle::headlineSmall() {
    return {
        .font = Media::Font{
            regularFontface(),
            24,
        },
    };
}

TextStyle TextStyle::titleLarge() {
    return {
        .font = Media::Font{
            regularFontface(),
            22,
        },
    };
}

TextStyle TextStyle::titleMedium() {
    return {
        .font = Media::Font{
            mediumFontface(),
            16,
        },
    };
}

TextStyle TextStyle::titleSmall() {
    return {
        .font = Media::Font{
            mediumFontface(),
            14,
        },
    };
}

TextStyle TextStyle::labelLarge() {
    return {
        .font = Media::Font{
            mediumFontface(),
            14,
        },
    };
}
TextStyle TextStyle::labelMedium() {
    return {
        .font = Media::Font{
            mediumFontface(),
            12,
        },
    };
}
TextStyle TextStyle::labelSmall() {
    return {
        .font = Media::Font{
            mediumFontface(),
            11,
        },
    };
}

TextStyle TextStyle::bodyLarge() {
    return {
        .font = Media::Font{
            regularFontface(),
            16,
        },
    };
}

TextStyle TextStyle::bodyMedium() {
    return {
        .font = Media::Font{
            regularFontface(),
            14,
        },
    };
}
TextStyle TextStyle::bodySmall() {
    return {
        .font = Media::Font{
            regularFontface(),
            12,
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
        auto baseline = bound().topStart() + m.baseline;

        if (_style.color) {
            g.fillStyle(*_style.color);
        }

        g.textFont(_style.font);
        g.fill(baseline, _text);

        if (debugShowLayoutBounds) {
            g.debugLine(
                {
                    bound().topStart() + m.baseline.cast<isize>(),
                    bound().topEnd() + m.baseline.cast<isize>(),
                },
                Gfx::PINK);
            g.debugRect(bound(), Gfx::CYAN);
        }

        g.restore();
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return mesure().linebound.size().cast<isize>();
    }
};

Child text(TextStyle style, Str text) {
    return makeStrong<Text>(style, text);
}

Child text(Str text) {
    return makeStrong<Text>(TextStyle::labelMedium(), text);
}

/* --- Badge ---------------------------------------------------------------- */

Child badge(Gfx::Color color, String t) {
    BoxStyle boxStyle = {
        .padding = {8, 4},
        .borderRadius = 99,
        .backgroundPaint = color.withOpacity(0.2),
        .foregroundPaint = color,
    };

    return text(t) | box(boxStyle);
}

Child badge(BadgeStyle style, String t) {
    Array COLORS = {
        Gfx::BLUE400,
        Gfx::LIME400,
        Gfx::YELLOW400,
        Gfx::RED400,
    };

    return badge(COLORS[static_cast<u8>(style)], t);
}

/* --- Icon ----------------------------------------------------------------- */

struct Icon : public View<Icon> {
    Media::Icon _icon;
    Opt<Gfx::Color> _color;

    Icon(Media::Icon icon, Opt<Gfx::Color> color)
        : _icon(icon), _color(color) {}

    void reconcile(Icon &o) override {
        _icon = o._icon;
        _color = o._color;
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();
        if (_color)
            g.fillStyle(_color.unwrap());
        g.fill(bound().topStart(), _icon);
        if (debugShowLayoutBounds)
            g.debugRect(bound(), Gfx::CYAN);
        g.restore();
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return _icon.bound().size().cast<isize>();
    }
};

Child icon(Media::Icon icon, Opt<Gfx::Color> color) {
    return makeStrong<Icon>(icon, color);
}

Child icon(Mdi::Icon i, f64 size, Opt<Gfx::Color> color) {
    return icon(Media::Icon{i, size}, color);
}

/* --- Image ---------------------------------------------------------------- */

struct Image : public View<Image> {
    Media::Image _image;
    Opt<Gfx::BorderRadius> _radius;

    Image(Media::Image image)
        : _image(image) {}

    Image(Media::Image image, Gfx::BorderRadius radius)
        : _image(image), _radius(radius) {}

    void paint(Gfx::Context &g, Math::Recti) override {
        if (_radius) {
            g.fillStyle(_image);
            g.fill(bound(), *_radius);
        } else {
            g.blit(bound(), _image);
        }

        if (debugShowLayoutBounds)
            g.debugRect(bound(), Gfx::CYAN);
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return _image.bound().size().cast<isize>();
    }
};

Child image(Media::Image image) {
    return makeStrong<Image>(image);
}

Child image(Media::Image image, Gfx::BorderRadius radius) {
    return makeStrong<Image>(image, radius);
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
        }

        return _bound.wh;
    }
};

Child canvas(OnPaint onPaint) {
    return makeStrong<Canvas>(std::move(onPaint));
}

/* --- Filter --------------------------------------------------------------- */

struct BackgroundFilter : public ProxyNode<BackgroundFilter> {
    Gfx::Filter _filter;

    BackgroundFilter(Gfx::Filter radius, Child child)
        : ProxyNode<BackgroundFilter>(std::move(child)),
          _filter(radius) {}

    void reconcile(BackgroundFilter &o) override {
        _filter = o._filter;
        ProxyNode<BackgroundFilter>::reconcile(o);
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        g.apply(_filter, bound());
        ProxyNode<BackgroundFilter>::paint(g, r);
    }
};

Child backgroundFilter(Gfx::Filter f, Child child) {
    return makeStrong<BackgroundFilter>(f, std::move(child));
}

struct ForegroundFilter : public ProxyNode<ForegroundFilter> {
    Gfx::Filter _filter;

    ForegroundFilter(Gfx::Filter radius, Child child)
        : ProxyNode<ForegroundFilter>(std::move(child)),
          _filter(radius) {}

    void reconcile(ForegroundFilter &o) override {
        _filter = o._filter;
        ProxyNode<ForegroundFilter>::reconcile(o);
    }

    void paint(Gfx::Context &g, Math::Recti r) override {
        ProxyNode<ForegroundFilter>::paint(g, r);
        g.apply(_filter, bound());
    }
};

Child foregroundFilter(Gfx::Filter f, Child child) {
    return makeStrong<ForegroundFilter>(f, std::move(child));
}

} // namespace Karm::Ui
