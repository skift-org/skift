#pragma once

#include <karm-ui/react.h>

namespace Settings {

/* --- Model ---------------------------------------------------------------- */

enum struct Page {
    HOME,
    ACCOUNT,
    PERSONALIZATION,
    APPLICATIONS,
    SYSTEM,
    NETWORK,
    SECURITY,
    UPDATES,
    ABOUT,
};

struct State {
    bool sidebarOpen = false;
    Page page = Page::HOME;
};

struct ToggleSidebar {};

struct GoTo {
    Page page;
};

using Actions = Var<ToggleSidebar, GoTo>;

State reduce(State s, Actions action);

using Model = Ui::Model<State, Actions, reduce>;

/* --- Views ---------------------------------------------------------------- */

Ui::Child pageScafold(Ui::Child inner);

Ui::Child pageHome(State const &);

Ui::Child pageAbout(State const &);

} // namespace Settings
