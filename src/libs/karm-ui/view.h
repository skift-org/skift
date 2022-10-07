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

Child text(Media::Font font, Str text);

Child text(int size, Str text);

Child text(Str text);

template <typename... Args>
inline Child text(Media::Font font, Str format, Args &&...args) {
    return text(font, Fmt::format(format, std::forward<Args>(args)...));
}

template <typename... Args>
inline Child text(int size, Str format, Args &&...args) {
    return text(size, Fmt::format(format, std::forward<Args>(args)...));
}

template <typename... Args>
inline Child text(Str format, Args &&...args) {
    return text(Fmt::format(format, std::forward<Args>(args)...));
}

/* --- Icon ----------------------------------------------------------------- */

Child icon(Media::Icon icon);

Child icon(Media::Icons icon, double size);

/* --- Image ---------------------------------------------------------------- */

struct Image : public View<Image> {
    Media::Image image;

    Image(Media::Image image)
        : image(image) {
    }

    void paint(Gfx::Context &g, Math::Recti) override {
        g.blit(bound(), image);
        if (DEBUG)
            g._rect(bound(), Gfx::CYAN);
    }

    Math::Vec2i size(Math::Vec2i, Layout::Hint) override {
        return image.bound().size().cast<int>();
    }
};

inline Child image(Media::Image image) {
    return makeStrong<Image>(image);
}

/* --- Canvas --------------------------------------------------------------- */

using OnPaint = Func<void(Gfx::Context &g, Math::Vec2i size)>;

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
};

inline Child canvas(OnPaint onPaint) {
    return makeStrong<Canvas>(std::move(onPaint));
}

} // namespace Karm::Ui
