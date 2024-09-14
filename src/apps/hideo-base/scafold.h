#pragma once

#include <karm-kira/titlebar.h>
#include <karm-media/icon.h>
#include <karm-sys/context.h>
#include <karm-ui/box.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/node.h>
#include <karm-ui/reducer.h>

namespace Hideo {

struct Scafold : Meta::NoCopy {
    Mdi::Icon icon;
    String title;
    Kr::TitlebarStyle titlebar = Kr::TitlebarStyle::DEFAULT;

    Opt<Ui::Slots> startTools = NONE;
    Opt<Ui::Slots> midleTools = NONE;
    Opt<Ui::Slots> endTools = NONE;
    Opt<Ui::Slot> sidebar = NONE;
    Ui::Slot body;

    Math::Vec2i size = {800, 600};

    struct State {
        bool sidebarOpen = false;
        bool isMobile = false;
    };

    struct ToggleSidebar {};

    using Action = Union<ToggleSidebar>;

    static void reduce(State &s, Action a) {
        if (a.is<ToggleSidebar>()) {
            s.sidebarOpen = !s.sidebarOpen;
        }
    }

    using Model = Ui::Model<State, Action, reduce>;
};

Ui::Child scafold(Scafold scafold);

} // namespace Hideo
