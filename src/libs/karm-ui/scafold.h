#pragma once

#include "box.h"
#include "layout.h"
#include "node.h"

namespace Karm::Ui {

enum struct TitlebarStyle {
    DEFAULT,
    FIXED,
    DIALOG,
};

Child controls(TitlebarStyle style = TitlebarStyle::DEFAULT);

Child titlebar(Media::Icons icon, String title, TitlebarStyle style = TitlebarStyle::DEFAULT);

enum struct BadgeStyle {
    INFO,
    SUCCESS,
    WARNING,
    ERROR,
};

Child badge(BadgeStyle style, String text);

Child toolbar(Children children);

inline Ui::Child toolbar(Meta::Same<Child> auto... children) { return toolbar({children...}); }

inline Child card(Child child) {
    return box({
                   .borderRadius = 4,
                   .backgroundColor = Gfx::ZINC800,
               },
               child);
}

inline Child card(Children children) { return card(vflow(children)); }

inline Child card(Meta::Same<Child> auto... children) { return card({children...}); }

} // namespace Karm::Ui
