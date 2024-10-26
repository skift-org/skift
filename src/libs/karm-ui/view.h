#pragma once

#include <karm-gfx/icon.h>
#include <karm-image/picture.h>
#include <karm-text/prose.h>

#include "node.h"

namespace Karm::Ui {

// MARK: Base ------------------------------------------------------------------

template <typename Crtp>
struct View : public LeafNode<Crtp> {
    Math::Recti _bound;

    Math::Recti bound() override {
        return _bound;
    }

    void layout(Math::Recti bound) override {
        _bound = bound;
    }
};

// MARK: Text ------------------------------------------------------------------

struct TextStyles {
private:
    TextStyles() = default;

public:
    static Text::ProseStyle displayLarge();
    static Text::ProseStyle displayMedium();
    static Text::ProseStyle displaySmall();

    static Text::ProseStyle headlineLarge();
    static Text::ProseStyle headlineMedium();
    static Text::ProseStyle headlineSmall();

    static Text::ProseStyle titleLarge();
    static Text::ProseStyle titleMedium();
    static Text::ProseStyle titleSmall();

    static Text::ProseStyle labelLarge();
    static Text::ProseStyle labelMedium();
    static Text::ProseStyle labelSmall();

    static Text::ProseStyle bodyLarge();
    static Text::ProseStyle bodyMedium();
    static Text::ProseStyle bodySmall();

    static Text::ProseStyle codeLarge();
    static Text::ProseStyle codeMedium();
    static Text::ProseStyle codeSmall();
};

Child text(Text::ProseStyle style, Str text);

Child text(Str text);

template <typename... Args>
inline Child text(Text::ProseStyle style, Str format, Args &&...args) {
    return text(style, Io::format(format, std::forward<Args>(args)...).unwrap());
}

template <typename... Args>
inline Child text(Str format, Args &&...args) {
    return text(Io::format(format, std::forward<Args>(args)...).unwrap());
}

#define DEF_STYLE(STYLE)                                                                                                  \
    inline Child STYLE(Str text) { return Karm::Ui::text(TextStyles::STYLE(), text); }                                    \
    inline Child STYLE(Gfx::Color color, Str text) { return Karm::Ui::text(TextStyles::STYLE().withColor(color), text); } \
    template <typename... Args>                                                                                           \
    inline Child STYLE(Str format, Args &&...args) {                                                                      \
        return text(TextStyles::STYLE(), format, std::forward<Args>(args)...);                                            \
    }                                                                                                                     \
    template <typename... Args>                                                                                           \
    inline Child STYLE(Gfx::Color color, Str format, Args &&...args) {                                                    \
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

Child icon(Gfx::Icon icon, Opt<Gfx::Color> color = NONE);

Child icon(Mdi::Icon icon, f64 size, Opt<Gfx::Color> color = NONE);

// MARK: Image -----------------------------------------------------------------

Child image(Image::Picture image);

Child image(Image::Picture image, Math::Radiif radii);

// MARK: Canvas ----------------------------------------------------------------

using OnPaint = Func<void(Gfx::Canvas &g, Math::Vec2i size)>;

Child canvas(OnPaint onPaint);

// MARK: Blur ------------------------------------------------------------------

Child backgroundFilter(Gfx::Filter f, Child child);

inline auto backgroundFilter(Gfx::Filter f) {
    return [=](Child child) {
        return backgroundFilter(f, child);
    };
}

Child foregroundFilter(Gfx::Filter f, Child child);

inline auto foregroundFilter(Gfx::Filter f) {
    return [=](Child child) {
        return foregroundFilter(f, child);
    };
}

} // namespace Karm::Ui
