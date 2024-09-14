#include <karm-app/host.h>
#include <karm-ui/dialog.h>
#include <karm-ui/drag.h>
#include <karm-ui/layout.h>

#include "about-dialog.h"
#include "titlebar.h"

namespace Karm::Kira {

Ui::Child titlebarTitle(Mdi::Icon icon, String title) {
    return Ui::button(
        [title](auto &n) {
            Ui::showDialog(n, Kr::aboutDialog(title));
        },
        Ui::ButtonStyle::subtle(), icon, title
    );
}

Ui::Child titlebarControls(TitlebarStyle style) {
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

Ui::Child titlebarContent(Ui::Children children) {
    return Ui::hflow(
               4,
               children
           ) |
           Ui::insets(8) |
           Ui::dragRegion() |
           Ui::box({.backgroundFill = Ui::GRAY900});
}

Ui::Child titlebar(Mdi::Icon icon, String title, TitlebarStyle style) {
    return titlebarContent({
        titlebarTitle(icon, title),
        Ui::grow(NONE),
        titlebarControls(style),
    });
}

Ui::Child titlebar(Mdi::Icon icon, String title, Ui::Child middle, TitlebarStyle style) {
    return titlebarContent({
        titlebarTitle(icon, title),
        middle | Ui::grow(),
        titlebarControls(style),
    });
}

} // namespace Karm::Kira
