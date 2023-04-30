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

Child titlebar(Mdi::Icon icon, String title, TitlebarStyle style = TitlebarStyle::DEFAULT);

Child titlebar(Mdi::Icon icon, String title, Child tabs, TitlebarStyle style = TitlebarStyle::DEFAULT);

enum struct BadgeStyle {
    INFO,
    SUCCESS,
    WARNING,
    ERROR,
};

Child badge(BadgeStyle style, String text);

Child toolbar(Children children);

inline Ui::Child toolbar(Meta::Same<Child> auto... children) { return toolbar({children...}); }

Ui::Child bottombar(Children children);

inline Ui::Child bottombar(Meta::Same<Child> auto... children) { return bottombar({children...}); }

inline Child card(Child child) {
    return box({
                   .borderRadius = 4,
                   .backgroundPaint = GRAY900,
               },
               child);
}

inline auto card() {
    return [](Child child) {
        return card(child);
    };
}

inline Child card(Children children) { return card(vflow(children)); }

inline Child card(Child child, Meta::Same<Child> auto... children) { return card({child, children...}); }

} // namespace Karm::Ui
