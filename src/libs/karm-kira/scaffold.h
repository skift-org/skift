#pragma once

#include <karm-kira/titlebar.h>
#include <karm-ui/reducer.h>

#include "_prelude.h"

namespace Karm::Kira {

struct Scaffold : Meta::NoCopy {
    Mdi::Icon icon;
    String title;
    Kr::TitlebarStyle titlebar = Kr::TitlebarStyle::DEFAULT;

    Opt<Ui::Slots> startTools = NONE;
    Opt<Ui::Slots> midleTools = NONE;
    Opt<Ui::Slots> endTools = NONE;
    Opt<Ui::Slot> sidebar = NONE;
    Ui::Slot body;

    Math::Vec2i size = {800, 600};
    bool compact = false;

    struct State {
        bool sidebarOpen = false;
        bool isMobile = false;
    };

    struct ToggleSidebar {};

    using Action = Union<ToggleSidebar>;

    static Ui::Task<Action> reduce(State& s, Action a) {
        if (a.is<ToggleSidebar>()) {
            s.sidebarOpen = !s.sidebarOpen;
        }

        return NONE;
    }

    using Model = Ui::Model<State, Action, reduce>;
};

Ui::Child scaffold(Scaffold scaffold);

} // namespace Karm::Kira
