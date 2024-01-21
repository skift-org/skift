#include <hideo-base/sidenav.h>
#include <karm-sys/entry.h>
#include <karm-ui/anim.h>
#include <karm-ui/app.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/reducer.h>
#include <karm-ui/scafold.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "app.h"

namespace Hideo::Settings {

Ui::Child sidebar(State const &state) {
    Ui::Children items = {
        Hideo::sidenavItem(state.page() == Page::ACCOUNT, Model::bind<GoTo>(Page::ACCOUNT), Mdi::ACCOUNT, "Accounts"),
        Hideo::sidenavItem(state.page() == Page::PERSONALIZATION, Model::bind<GoTo>(Page::PERSONALIZATION), Mdi::PALETTE, "Personalization"),
        Hideo::sidenavItem(state.page() == Page::APPLICATIONS, Model::bind<GoTo>(Page::APPLICATIONS), Mdi::WIDGETS_OUTLINE, "Applications"),

        Hideo::sidenavItem(state.page() == Page::SYSTEM, Model::bind<GoTo>(Page::SYSTEM), Mdi::LAPTOP, "System"),
        Hideo::sidenavItem(state.page() == Page::NETWORK, Model::bind<GoTo>(Page::NETWORK), Mdi::WIFI, "Network"),
        Hideo::sidenavItem(state.page() == Page::SECURITY, Model::bind<GoTo>(Page::SECURITY), Mdi::SECURITY, "Security & Privacy"),

        Hideo::sidenavItem(state.page() == Page::UPDATES, Model::bind<GoTo>(Page::UPDATES), Mdi::UPDATE, "Updates"),
        Hideo::sidenavItem(state.page() == Page::ABOUT, Model::bind<GoTo>(Page::ABOUT), Mdi::INFORMATION_OUTLINE, "About"),
    };

    return Hideo::sidenav(items);
}

/* --- Pages ---------------------------------------------------------------- */

Ui::Child pageContent(State const &state) {
    switch (state.page()) {
    case Page::HOME:
        return pageHome(state);

    case Page::ABOUT:
        return pageAbout(state);

    default:
        return Ui::grow(
            Ui::center(
                Ui::text("Content")));
    }
}

/* --- Body ----------------------------------------------------------------- */

Ui::Child app() {
    return Ui::reducer<Model>({}, [](State const &s) {
        return Ui::scafold({
            .icon = Mdi::COG,
            .title = "Settings",
            .startTools = slots$(
                Ui::button(Model::bindIf<GoBack>(s.canGoBack()), Ui::ButtonStyle::subtle(), Mdi::ARROW_LEFT),
                Ui::button(Model::bindIf<GoForward>(s.canGoForward()), Ui::ButtonStyle::subtle(), Mdi::ARROW_RIGHT),
                Ui::button(Model::bind<GoTo>(Page::HOME), Ui::ButtonStyle::subtle(), Mdi::HOME)),
            .sidebar = slot$(sidebar(s)),
            .body = slot$(pageContent(s) | Ui::grow()),
        });
    });
}

} // namespace Hideo::Settings

Res<> entryPoint(Sys::Ctx &ctx) {
    return Ui::runApp(ctx, Hideo::Settings::app());
}
