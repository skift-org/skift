#pragma once

#include <karm-gfx/text.h>

#include "node.h"

namespace Karm::Ui {

/* --- Base ----------------------------------------------------------------- */

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

/* --- Text ----------------------------------------------------------------- */

struct TextStyles {
private:
    TextStyles() = default;

public:
    static Gfx::TextStyle displayLarge();
    static Gfx::TextStyle displayMedium();
    static Gfx::TextStyle displaySmall();

    static Gfx::TextStyle headlineLarge();
    static Gfx::TextStyle headlineMedium();
    static Gfx::TextStyle headlineSmall();

    static Gfx::TextStyle titleLarge();
    static Gfx::TextStyle titleMedium();
    static Gfx::TextStyle titleSmall();

    static Gfx::TextStyle labelLarge();
    static Gfx::TextStyle labelMedium();
    static Gfx::TextStyle labelSmall();

    static Gfx::TextStyle bodyLarge();
    static Gfx::TextStyle bodyMedium();
    static Gfx::TextStyle bodySmall();

    static Gfx::TextStyle codeLarge();
    static Gfx::TextStyle codeMedium();
    static Gfx::TextStyle codeSmall();
};

Child text(Gfx::TextStyle style, Str text);

Child text(Str text);

template <typename... Args>
inline Child text(Gfx::TextStyle style, Str format, Args &&...args) {
    return text(style, Fmt::format(format, std::forward<Args>(args)...).unwrap());
}

template <typename... Args>
inline Child text(Str format, Args &&...args) {
    return text(Fmt::format(format, std::forward<Args>(args)...).unwrap());
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

/* --- Badge ---------------------------------------------------------------- */

enum struct BadgeStyle {
    INFO,
    SUCCESS,
    WARNING,
    ERROR,

    _LEN
};

Child badge(Gfx::Color color, String text);

Child badge(BadgeStyle style, String text);

/* --- Icon ----------------------------------------------------------------- */

Child icon(Media::Icon icon, Opt<Gfx::Color> color = NONE);

Child icon(Mdi::Icon icon, f64 size, Opt<Gfx::Color> color = NONE);

/* --- Image ---------------------------------------------------------------- */

Child image(Media::Image image);

Child image(Media::Image image, Gfx::BorderRadius radius);

/* --- Canvas --------------------------------------------------------------- */

using OnPaint = Func<void(Gfx::Context &g, Math::Vec2i size)>;

Child canvas(OnPaint onPaint);

/* --- Blur ----------------------------------------------------------------- */

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
