#pragma once

#include <karm-ui/box.h>
#include <karm-ui/layout.h>
#include <karm-ui/node.h>
#include <karm-ui/react.h>

namespace Shell {

/* --- Model ---------------------------------------------------------------- */

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
    CALENDAR,
    NOTIS,
};

struct State {
    bool isTablet = true;
    bool locked = true;
    Panel activePanel = Panel::NIL;
};

struct ToggleTablet {};

struct Unlock {};

struct Activate {
    Panel panel;
};

using Actions = Var<ToggleTablet, Unlock, Activate>;

State reduce(State state, Actions action);

using Model = Ui::Model<State, Actions, reduce>;

/* --- Views ---------------------------------------------------------------- */

inline auto panel() {
    return Ui::pinSize({500, 400}) |
           Ui::box({
               .padding = 8,
               .borderRadius = 8,
               .borderWidth = 1,
               .borderPaint = Gfx::ZINC800,
               .backgroundPaint = Gfx::ZINC950,
           });
}

Ui::Child lock(State const &state);

Ui::Child appsPanel();

Ui::Child appsFlyout();

} // namespace Shell
