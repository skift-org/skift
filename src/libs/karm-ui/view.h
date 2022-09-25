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
};

inline Child image() {
    return makeStrong<Image>();
}

} // namespace Karm::Ui
