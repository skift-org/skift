#pragma once

#include <karm-ui/react.h>

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
    bool sidebarOpen = true;
    Page page = Page::HOME;
};

struct ToggleSidebar {};

struct GoTo {
    Page page;
};

using Actions = Var<ToggleSidebar, GoTo>;

State reduce(State d, Actions action);

using Model = Ui::Model<State, Actions>;