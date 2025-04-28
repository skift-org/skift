module;

#include <karm-base/func.h>
#include <karm-gfx/icon.h>
#include <karm-image/picture.h>
#include <karm-scene/base.h>
#include <karm-text/loader.h>
#include <karm-text/prose.h>

export module Karm.Ui:view;

import :node;

namespace Karm::Ui {

// MARK: Base ------------------------------------------------------------------

export template <typename Crtp>
struct View : LeafNode<Crtp> {
    Math::Recti _bound;

    Math::Recti bound() override {
        return _bound;
    }

    void layout(Math::Recti bound) override {
        _bound = bound;
    }
};

// MARK: Text ------------------------------------------------------------------

static Opt<Rc<Text::Fontface>> _regularFontface = NONE;

Rc<Text::Fontface> regularFontface() {
    if (not _regularFontface) {
        _regularFontface = Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Regular.ttf"_url).unwrap();
    }
    return *_regularFontface;
}

static Opt<Rc<Text::Fontface>> _mediumFontface = NONE;

Rc<Text::Fontface> mediumFontface() {
    if (not _mediumFontface) {
        _mediumFontface = Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Medium.ttf"_url).unwrap();
    }
    return *_mediumFontface;
}

static Opt<Rc<Text::Fontface>> _boldFontface = NONE;

Rc<Text::Fontface> boldFontface() {
    if (not _boldFontface) {
        _boldFontface = Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Bold.ttf"_url).unwrap();
    }
    return *_boldFontface;
}

static Opt<Rc<Text::Fontface>> _italicFontface = NONE;

Rc<Text::Fontface> italicFontface() {
    if (not _italicFontface) {
        _italicFontface = Text::loadFontfaceOrFallback("bundle://fonts-inter/fonts/Inter-Italic.ttf"_url).unwrap();
    }
    return *_italicFontface;
}

static Opt<Rc<Text::Fontface>> _codeFontface = NONE;

Rc<Text::Fontface> codeFontface() {
    if (not _codeFontface) {
        _codeFontface = Text::loadFontfaceOrFallback("bundle://fonts-fira-code/fonts/FiraCode-Regular.ttf"_url).unwrap();
    }
    return *_codeFontface;
}

export struct TextStyles {
private:
    TextStyles() = default;

public:
    static Text::ProseStyle displayLarge() {
        return {
            .font = Text::Font{
                regularFontface(),
                57,
            },
        };
    }

    static Text::ProseStyle displayMedium() {
        return {
            .font = Text::Font{
                regularFontface(),
                45,
            },
        };
    }

    static Text::ProseStyle displaySmall() {
        return {
            .font = Text::Font{
                regularFontface(),
                36,
            },
        };
    }

    static Text::ProseStyle headlineLarge() {
        return {
            .font = Text::Font{
                regularFontface(),
                32,
            },
        };
    }

    static Text::ProseStyle headlineMedium() {
        return {
            .font = Text::Font{
                regularFontface(),
                28,
            },
        };
    }

    static Text::ProseStyle headlineSmall() {
        return {
            .font = Text::Font{
                regularFontface(),
                24,
            },
        };
    }

    static Text::ProseStyle titleLarge() {
        return {
            .font = Text::Font{
                regularFontface(),
                22,
            },
        };
    }

    static Text::ProseStyle titleMedium() {
        return {
            .font = Text::Font{
                mediumFontface(),
                16,
            },
        };
    }

    static Text::ProseStyle titleSmall() {
        return {
            .font = Text::Font{
                mediumFontface(),
                14,
            },
        };
    }

    static Text::ProseStyle labelLarge() {
        return {
            .font = Text::Font{
                mediumFontface(),
                14,
            },
        };
    }

    static Text::ProseStyle labelMedium() {
        return {
            .font = Text::Font{
                mediumFontface(),
                12,
            },
        };
    }

    static Text::ProseStyle labelSmall() {
        return {
            .font = Text::Font{
                mediumFontface(),
                11,
            },
        };
    }

    static Text::ProseStyle bodyLarge() {
        return {
            .font = Text::Font{
                regularFontface(),
                16,
            },
            .multiline = true,
        };
    }

    static Text::ProseStyle bodyMedium() {
        return {
            .font = Text::Font{
                regularFontface(),
                14,
            },
            .multiline = true,
        };
    }

    static Text::ProseStyle bodySmall() {
        return {
            .font = Text::Font{
                regularFontface(),
                12,
            },
            .multiline = true,
        };
    }

    static Text::ProseStyle codeLarge() {
        return {
            .font = Text::Font{
                codeFontface(),
                16,
            },
            .multiline = true,
        };
    }

    static Text::ProseStyle codeMedium() {
        return {
            .font = Text::Font{
                codeFontface(),
                14,
            },
            .multiline = true,
        };
    }

    static Text::ProseStyle codeSmall() {
        return {
            .font = Text::Font{
                codeFontface(),
                12,
            },
            .multiline = true,
        };
    }
};

struct Text : View<Text> {
    Rc<Karm::Text::Prose> _prose;

    Text(Rc<Karm::Text::Prose> prose)
        : _prose(std::move(prose)) {}

    Text(::Text::ProseStyle style, Str text)
        : _prose(makeRc<Karm::Text::Prose>(style, text)) {}

    void reconcile(Text& o) override {
        _prose = std::move(o._prose);
    }

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();
        g.origin(bound().xy.cast<f64>());
        g.fill(*_prose);
        g.pop();
    }

    void layout(Math::Recti bound) override {
        _prose->layout(Au{bound.width});
        View<Text>::layout(bound);
    }

    Math::Vec2i size(Math::Vec2i s, Hint) override {
        auto size = _prose->layout(Au{s.width});
        return size.ceil().cast<isize>();
    }
};

export Child text(Karm::Text::ProseStyle style, Str text) {
    return makeRc<Text>(style, text);
}

export Child text(Str text) {
    return makeRc<Text>(TextStyles::labelMedium(), text);
}

export Child text(Rc<Karm::Text::Prose> prose) {
    return makeRc<Text>(prose);
}

export template <typename... Args>
Child text(Karm::Text::ProseStyle style, Str format, Args&&... args) {
    return text(style, Io::format(format, std::forward<Args>(args)...));
}

export template <typename... Args>
Child text(Str format, Args&&... args) {
    return text(Io::format(format, std::forward<Args>(args)...));
}

#define DEF_STYLE(STYLE)                                                                                                  \
    export Child STYLE(Str text) { return Karm::Ui::text(TextStyles::STYLE(), text); }                                    \
    export Child STYLE(Gfx::Color color, Str text) { return Karm::Ui::text(TextStyles::STYLE().withColor(color), text); } \
    export template <typename... Args>                                                                                    \
    inline Child STYLE(Str format, Args&&... args) {                                                                      \
        return text(TextStyles::STYLE(), format, std::forward<Args>(args)...);                                            \
    }                                                                                                                     \
    export template <typename... Args>                                                                                    \
    inline Child STYLE(Gfx::Color color, Str format, Args&&... args) {                                                    \
        return text(TextStyles::STYLE().withColor(color), format, std::forward<Args>(args)...);                           \
    }

DEF_STYLE(displayLarge)
DEF_STYLE(displayMedium)
DEF_STYLE(displaySmall)
DEF_STYLE(headlineLarge)
DEF_STYLE(headlineMedium)
DEF_STYLE(headlineSmall)
DEF_STYLE(titleLarge)
DEF_STYLE(titleMedium)
DEF_STYLE(titleSmall)
DEF_STYLE(labelLarge)
DEF_STYLE(labelMedium)
DEF_STYLE(labelSmall)
DEF_STYLE(bodyLarge)
DEF_STYLE(bodyMedium)
DEF_STYLE(bodySmall)
DEF_STYLE(codeLarge)
DEF_STYLE(codeMedium)
DEF_STYLE(codeSmall)

#undef DEF_STYLE

// MARK: Icon ------------------------------------------------------------------

struct Icon : View<Icon> {
    Gfx::Icon _icon;
    isize _size;
    Opt<Gfx::Color> _color;

    Icon(Gfx::Icon icon, isize size, Opt<Gfx::Color> color = NONE)
        : _icon(icon), _size(size), _color(color) {}

    void reconcile(Icon& o) override {
        _icon = o._icon;
        _color = o._color;
    }

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();
        if (_color)
            g.fillStyle(_color.unwrap());
        _icon.fill(g, bound().topStart().cast<f64>(), _size);
        g.pop();
    }

    Math::Vec2i size(Math::Vec2i, Hint) override {
        return Math::Vec2i{_size, _size};
    }
};

export Child icon(Gfx::Icon icon, Opt<Gfx::Color> color = NONE) {
    return makeRc<Icon>(icon, 18, color);
}

export Child icon(Gfx::Icon i, isize size, Opt<Gfx::Color> color = NONE) {
    return makeRc<Icon>(i, size, color);
}

// MARK: Image -----------------------------------------------------------------

struct Image : View<Image> {
    Karm::Image::Picture _image;
    Opt<Math::Radiif> _radii;

    Image(Karm::Image::Picture image, Opt<Math::Radiif> radii = NONE)
        : _image(image), _radii(radii) {}

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();

        if (_radii) {
            g.fillStyle(_image.pixels());
            g.fill(bound(), *_radii);
        } else {
            g.blit(bound(), _image);
        }

        g.pop();
    }

    Math::Vec2i size(Math::Vec2i, Hint) override {
        return _image.bound().size().cast<isize>();
    }
};

export Child image(Karm::Image::Picture image) {
    return makeRc<Image>(image);
}

export Child image(Karm::Image::Picture image, Math::Radiif radii) {
    return makeRc<Image>(image, radii);
}

// MARK: Canvas ----------------------------------------------------------------

export using OnPaint = Func<void(Gfx::Canvas& g, Math::Vec2i size)>;

struct Canvas : View<Canvas> {
    OnPaint _onPaint;

    Canvas(OnPaint onPaint)
        : _onPaint(std::move(onPaint)) {}

    void reconcile(Canvas& o) override {
        _onPaint = std::move(o._onPaint);
        View<Canvas>::reconcile(o);
    }

    void paint(Gfx::Canvas& g, Math::Recti) override {
        g.push();
        g.clip(_bound);
        g.origin(_bound.xy.cast<f64>());
        _onPaint(g, _bound.wh);
        g.pop();
    }

    Math::Vec2i size(Math::Vec2i, Hint hint) override {
        if (hint == Hint::MIN)
            return 0;
        return _bound.wh;
    }
};

export Child canvas(OnPaint onPaint) {
    return makeRc<Canvas>(std::move(onPaint));
}

struct SceneCanvas : View<SceneCanvas> {
    Rc<Scene::Node> _scene;
    Scene::PaintOptions _options;

    SceneCanvas(Rc<Scene::Node> scene, Scene::PaintOptions options)
        : _scene(std::move(scene)), _options(options) {}

    void reconcile(SceneCanvas& o) override {
        _scene = o._scene;
        _options = o._options;
        View<SceneCanvas>::reconcile(o);
    }

    void paint(Gfx::Canvas& g, Math::Recti rect) override {
        g.push();
        g.clip(_bound);
        g.origin(_bound.xy.cast<f64>());
        g.scale(_bound.size().cast<f64>() / _scene->bound().size().cast<f64>());

        Math::Trans2f trans;
        trans = trans.translated(-_bound.xy.cast<f64>());
        trans = trans.scaled(_bound.size().cast<f64>() / _scene->bound().size().cast<f64>());

        auto rectInScene = trans.inverse().apply(rect.cast<f64>()).bound();

        _scene->paint(g, Math::Rectf::MAX, _options);

        g.pop();
    }

    Math::Vec2i size(Math::Vec2i, Hint hint) override {
        if (hint == Hint::MIN) {
            return 0;
        }

        return _scene->bound().size().ceil().cast<isize>();
    }
};

export Child canvas(Rc<Scene::Node> child, Scene::PaintOptions options) {
    return makeRc<SceneCanvas>(std::move(child), options);
}

// MARK: Blur ------------------------------------------------------------------

struct BackgroundFilter : ProxyNode<BackgroundFilter> {
    Gfx::Filter _filter;

    BackgroundFilter(Gfx::Filter filter, Child child)
        : ProxyNode<BackgroundFilter>(std::move(child)),
          _filter(filter) {}

    void reconcile(BackgroundFilter& o) override {
        _filter = o._filter;
        ProxyNode<BackgroundFilter>::reconcile(o);
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        g.push();
        g.clip(bound());
        g.apply(_filter);
        g.pop();
        ProxyNode<BackgroundFilter>::paint(g, r);
    }
};

export Child backgroundFilter(Gfx::Filter f, Child child) {
    return makeRc<BackgroundFilter>(f, std::move(child));
}

export auto backgroundFilter(Gfx::Filter f) {
    return [=](Child child) {
        return backgroundFilter(f, child);
    };
}

struct ForegroundFilter : ProxyNode<ForegroundFilter> {
    Gfx::Filter _filter;

    ForegroundFilter(Gfx::Filter filter, Child child)
        : ProxyNode<ForegroundFilter>(std::move(child)),
          _filter(filter) {}

    void reconcile(ForegroundFilter& o) override {
        _filter = o._filter;
        ProxyNode<ForegroundFilter>::reconcile(o);
    }

    void paint(Gfx::Canvas& g, Math::Recti r) override {
        ProxyNode<ForegroundFilter>::paint(g, r);
        g.push();
        g.clip(bound());
        g.apply(_filter);
        g.pop();
    }
};

export Child foregroundFilter(Gfx::Filter f, Child child) {
    return makeRc<ForegroundFilter>(f, std::move(child));
}

export auto foregroundFilter(Gfx::Filter f) {
    return [=](Child child) {
        return foregroundFilter(f, child);
    };
}

} // namespace Karm::Ui
