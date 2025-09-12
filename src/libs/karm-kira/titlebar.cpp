module;

#include <karm-gfx/icon.h>

export module Karm.Kira:titlebar;

import Karm.App;
import Karm.Ui;
import Mdi;
import :aboutDialog;

namespace Karm::Kira {

export enum struct TitlebarStyle {
    DEFAULT,
    FIXED,
    DIALOG
};

export Ui::Child titlebarTitle(Gfx::Icon icon, String title, bool compact = false) {
    if (compact) {
        return Ui::button(
            [=](Ui::Node& n) {
                Ui::showDialog(n, aboutDialog(title));
            },
            Ui::ButtonStyle::subtle(),
            icon
        );
    }

    return Ui::button(
        [=](auto& n) {
            Ui::showDialog(n, aboutDialog(title));
        },
        Ui::ButtonStyle::subtle(), icon, title
    );
}

export Ui::Child titlebarControls(TitlebarStyle style) {
    return Ui::hflow(
        4,
        Ui::button(
            Ui::bindBubble<App::RequestMinimizeEvent>(),
            Ui::ButtonStyle::subtle(),
            Mdi::MINUS
        ) | Ui::cond(style == TitlebarStyle::DEFAULT),
        Ui::button(
            Ui::bindBubble<App::RequestMaximizeEvent>(),
            Ui::ButtonStyle::subtle(),
            Mdi::CROP_SQUARE
        ) | Ui::cond(style == TitlebarStyle::DEFAULT),
        Ui::button(
            Ui::bindBubble<App::RequestExitEvent>(),
            Ui::ButtonStyle::subtle(),
            Mdi::CLOSE
        )
    );
}

export struct TitlebarContent {
    Ui::Child start;
    Ui::Child middle = Ui::empty();
    Ui::Child end;

    operator Ui::Child() const {
        return Ui::hflow(
                   4,
                   start | Ui::insets({8, 0}),
                   middle | Ui::grow(),
                   end | Ui::insets({8, 0})
               ) |
               Ui::insets({0, 8}) |
               Ui::dragRegion();
    }
};

export Ui::Child titlebar(Gfx::Icon icon, String title, TitlebarStyle style = TitlebarStyle::DEFAULT) {
    return TitlebarContent{
        .start = titlebarTitle(icon, title),
        .end = titlebarControls(style)
    };
}

export Ui::Child titlebar(Gfx::Icon icon, String title, Ui::Child middle, TitlebarStyle style = TitlebarStyle::DEFAULT) {
    return TitlebarContent{
        .start = titlebarTitle(icon, title),
        .middle = middle,
        .end = titlebarControls(style),
    };
}

} // namespace Karm::Kira
