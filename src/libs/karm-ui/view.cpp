#include "view.h"

#include "box.h"

namespace Karm::Ui {

// MARK: Text ------------------------------------------------------------------

static Opt<Strong<Media::Fontface>> _regularFontface = NONE;
Strong<Media::Fontface> regularFontface() {
    if (not _regularFontface) {
        _regularFontface = Media::loadFontfaceOrFallback("bundle://inter-font/fonts/Inter-Regular.ttf"_url).unwrap();
    }
    return *_regularFontface;
}

static Opt<Strong<Media::Fontface>> _mediumFontface = NONE;
Strong<Media::Fontface> mediumFontface() {
    if (not _mediumFontface) {
        _mediumFontface = Media::loadFontfaceOrFallback("bundle://inter-font/fonts/Inter-Medium.ttf"_url).unwrap();
    }
    return *_mediumFontface;
}

static Opt<Strong<Media::Fontface>> _boldFontface = NONE;
Strong<Media::Fontface> boldFontface() {
    if (not _boldFontface) {
        _boldFontface = Media::loadFontfaceOrFallback("bundle://inter-font/fonts/Inter-Bold.ttf"_url).unwrap();
    }
    return *_boldFontface;
}

static Opt<Strong<Media::Fontface>> _italicFontface = NONE;
Strong<Media::Fontface> italicFontface() {
    if (not _italicFontface) {
        _italicFontface = Media::loadFontfaceOrFallback("bundle://inter-font/fonts/Inter-Italic.ttf"_url).unwrap();
    }
    return *_italicFontface;
}

static Opt<Strong<Media::Fontface>> _codeFontface = NONE;
Strong<Media::Fontface> codeFontface() {
    if (not _codeFontface) {
        _codeFontface = Media::loadFontfaceOrFallback("bundle://fira-code-font/fonts/FiraCode-Regular.ttf"_url).unwrap();
    }
    return *_codeFontface;
}

Gfx::TextStyle TextStyles::displayLarge() {
    return {
        .font = Media::Font{
            regularFontface(),
            57,
        },
    };
}

Gfx::TextStyle TextStyles::displayMedium() {
    return {
        .font = Media::Font{
            regularFontface(),
            45,
        },
    };
}

Gfx::TextStyle TextStyles::displaySmall() {
    return {
        .font = Media::Font{
            regularFontface(),
            36,
        },
    };
}

Gfx::TextStyle TextStyles::headlineLarge() {
    return {
        .font = Media::Font{
            regularFontface(),
            32,
        },
    };
}

Gfx::TextStyle TextStyles::headlineMedium() {
    return {
        .font = Media::Font{
            regularFontface(),
            28,
        },
    };
}

Gfx::TextStyle TextStyles::headlineSmall() {
    return {
        .font = Media::Font{
            regularFontface(),
            24,
        },
    };
}

Gfx::TextStyle TextStyles::titleLarge() {
    return {
        .font = Media::Font{
            regularFontface(),
            22,
        },
    };
}

Gfx::TextStyle TextStyles::titleMedium() {
    return {
        .font = Media::Font{
            mediumFontface(),
            16,
        },
    };
}

Gfx::TextStyle TextStyles::titleSmall() {
    return {
        .font = Media::Font{
            mediumFontface(),
            14,
        },
    };
}

Gfx::TextStyle TextStyles::labelLarge() {
    return {
        .font = Media::Font{
            mediumFontface(),
            14,
        },
    };
}

Gfx::TextStyle TextStyles::labelMedium() {
    return {
        .font = Media::Font{
            mediumFontface(),
            12,
        },
    };
}

Gfx::TextStyle TextStyles::labelSmall() {
    return {
        .font = Media::Font{
            mediumFontface(),
            11,
        },
    };
}

Gfx::TextStyle TextStyles::bodyLarge() {
    return {
        .font = Media::Font{
            regularFontface(),
            16,
        },
        .multiline = true,
    };
}

Gfx::TextStyle TextStyles::bodyMedium() {
    return {
        .font = Media::Font{
            regularFontface(),
            14,
        },
        .multiline = true,
    };
}

Gfx::TextStyle TextStyles::bodySmall() {
    return {
        .font = Media::Font{
            regularFontface(),
            12,
        },
        .multiline = true,
    };
}

Gfx::TextStyle TextStyles::codeLarge() {
    return {
        .font = Media::Font{
            codeFontface(),
            16,
        },
        .multiline = true,
    };
}

Gfx::TextStyle TextStyles::codeMedium() {
    return {
        .font = Media::Font{
            codeFontface(),
            14,
        },
        .multiline = true,
    };
}

Gfx::TextStyle TextStyles::codeSmall() {
    return {
        .font = Media::Font{
            codeFontface(),
            12,
        },
        .multiline = true,
    };
}

struct Text : public View<Text> {
    Gfx::Text _text;

    Text(Gfx::TextStyle style, Str text)
        : _text(style, text) {}

    void reconcile(Text &o) override {
        _text = o._text;
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();
        g.origin(bound().xy);
        _text.paint(g);
        g.restore();
        if (debugShowLayoutBounds)
            g.plot(bound(), Gfx::CYAN);
    }

    void layout(Math::Recti bound) override {
        _text.layout(bound.width);
        View<Text>::layout(bound);
    }

    Math::Vec2i size(Math::Vec2i s, Hint) override {
        auto size = _text.layout(s.width);
        return size.ceil().cast<isize>();
    }
};

Child text(Gfx::TextStyle style, Str text) {
    return makeStrong<Text>(style, text);
}

Child text(Str text) {
    return makeStrong<Text>(TextStyles::labelMedium(), text);
}

// MARK: Badge -----------------------------------------------------------------

Child badge(Gfx::Color color, String t) {
    BoxStyle boxStyle = {
        .padding = {6, 2},
        .borderRadius = 99,
        .backgroundPaint = color.withOpacity(0.2),
        .foregroundPaint = color,
    };

    return labelSmall(t) | box(boxStyle);
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

// MARK: Icon ------------------------------------------------------------------

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
            g.plot(bound(), Gfx::CYAN);
        g.restore();
    }

    Math::Vec2i size(Math::Vec2i, Hint) override {
        return _icon.bound().size().cast<isize>();
    }
};

Child icon(Media::Icon icon, Opt<Gfx::Color> color) {
    return makeStrong<Icon>(icon, color);
}

Child icon(Mdi::Icon i, f64 size, Opt<Gfx::Color> color) {
    return icon(Media::Icon{i, size}, color);
}

// MARK: Image -----------------------------------------------------------------

struct Image : public View<Image> {
    Media::Image _image;
    Opt<Math::Radiusf> _radius;

    Image(Media::Image image)
        : _image(image) {}

    Image(Media::Image image, Math::Radiusf radius)
        : _image(image), _radius(radius) {}

    void paint(Gfx::Context &g, Math::Recti) override {
        g.save();

        if (_radius) {
            g.fillStyle(_image);
            g.fill(bound(), *_radius);
        } else {
            g.blit(bound(), _image);
        }

        if (debugShowLayoutBounds)
            g.plot(bound(), Gfx::CYAN);
        g.restore();
    }

    Math::Vec2i size(Math::Vec2i, Hint) override {
        return _image.bound().size().cast<isize>();
    }
};

Child image(Media::Image image) {
    return makeStrong<Image>(image);
}

Child image(Media::Image image, Math::Radiusf radius) {
    return makeStrong<Image>(image, radius);
}

// MARK: Canvas ----------------------------------------------------------------

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

    Math::Vec2i size(Math::Vec2i, Hint hint) override {
        if (hint == Hint::MIN) {
            return 0;
        }

        return _bound.wh;
    }
};

Child canvas(OnPaint onPaint) {
    return makeStrong<Canvas>(std::move(onPaint));
}

// MARK: Filter ----------------------------------------------------------------

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
