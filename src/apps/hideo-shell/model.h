#pragma once

#include <karm-ui/reducer.h>

namespace Shell {

struct App {
    Mdi::Icon icon;
    Gfx::ColorRamp color;
    String name;
};

struct Noti {
    String title;
    String msg;
};

enum struct Panel {
    NIL,
    APPS,
    NOTIS,
    SYS,
};

struct State {
    bool locked = true;
    bool isMobile = true;
    Panel activePanel = Panel::NIL;
    bool isSysPanelColapsed = true;
};

struct ToggleTablet {};

struct Lock {};

struct Unlock {};

struct Activate {
    Panel panel;
};

struct ToggleSysPanel {};

using Actions = Var<ToggleTablet, Lock, Unlock, Activate, ToggleSysPanel>;

State reduce(State state, Actions action);

using Model = Ui::Model<State, Actions, reduce>;

} // namespace Shell
