#pragma once

#include <karm-ui/reducer.h>

namespace Shell {

struct MenuIcon {
    Mdi::Icon icon;
    Gfx::ColorRamp colors;
};

struct MenuEntry {
    MenuIcon icon;
    String name;
};

struct Noti {
    usize id;
    MenuEntry entry;
    String title;
    String body;
    Vec<String> actions{};
};

enum struct Panel {
    NIL,
    APPS,
    NOTIS,
    SYS,
};

struct Surface {
    usize id;
    MenuEntry entry;
    Gfx::Color color;
};

struct State {
    bool locked = true;
    bool isMobile = true;
    Panel activePanel = Panel::NIL;
    bool isSysPanelColapsed = true;

    Media::Image background;
    Vec<Noti> noti;
    Vec<MenuEntry> entries;
    Vec<Surface> surfaces;
};

struct ToggleTablet {};

struct Lock {};

struct Unlock {};

struct DimisNoti {
    usize index;
};

struct StartApp {
    usize index;
};

struct CloseApp {
    usize index;
};

struct FocusApp {
    usize index;
};

struct ToggleSysPanel {};

struct Activate {
    Panel panel;
};

using Action = Var<ToggleTablet, Lock, Unlock, DimisNoti, StartApp, CloseApp, FocusApp, Activate, ToggleSysPanel>;

void reduce(State &, Action);

using Model = Ui::Model<State, Action, reduce>;

} // namespace Shell
