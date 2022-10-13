#pragma once

#include "node.h"

namespace Karm::Ui {

/* --- Base ----------------------------------------------------------------- */

template <typename Crtp>
struct View : public Widget<Crtp> {
    Math::Recti _bound;

    Math::Recti bound() override {
        return _bound;
    }

    void layout(Math::Recti bound) override {
        _bound = bound;
    }
};

/* --- Text ----------------------------------------------------------------- */

struct TextStyle {
    Media::Font font;
    Opt<Gfx::Color> color;

    TextStyle withSize(int size) {
        TextStyle style = *this;
        style.font._size = size;
        return style;
    }

    TextStyle withColor(Gfx::Color color) {
        TextStyle style = *this;
        style.color = color;
        return style;
    }

    static TextStyle regular();
    static TextStyle bold();
    static TextStyle italic();

    static TextStyle title1();
    static TextStyle title2();
    static TextStyle title3();
    static TextStyle subtitle1();
    static TextStyle subtitle2();
    static TextStyle label();
    static TextStyle body();
};

Child text(TextStyle style, Str text);

Child text(Str text);

template <typename... Args>
inline Child text(TextStyle style, Str format, Args &&...args) {
    return text(style, Fmt::format(format, std::forward<Args>(args)...));
}

template <typename... Args>
inline Child text(Str format, Args &&...args) {
    return text(Fmt::format(format, std::forward<Args>(args)...));
}

/* --- Icon ----------------------------------------------------------------- */

Child icon(Media::Icon icon);

Child icon(Media::Icons icon, double size);

/* --- Image ---------------------------------------------------------------- */

Child image(Media::Image image);

/* --- Canvas --------------------------------------------------------------- */

using OnPaint = Func<void(Gfx::Context &g, Math::Vec2i size)>;

Child canvas(OnPaint onPaint);

} // namespace Karm::Ui
