#pragma once

#include <karm-media/icon.h>
#include <karm-sys/context.h>
#include <karm-ui/box.h>
#include <karm-ui/dialog.h>
#include <karm-ui/layout.h>
#include <karm-ui/node.h>
#include <karm-ui/reducer.h>

namespace Hideo {

enum struct TitlebarStyle {
    DEFAULT,
    FIXED,
    DIALOG,
};

Ui::Child controls(TitlebarStyle style = TitlebarStyle::DEFAULT);

Ui::Child titlebar(Mdi::Icon icon, String title, TitlebarStyle style = TitlebarStyle::DEFAULT);

Ui::Child titlebar(Mdi::Icon icon, String title, Ui::Child tabs, TitlebarStyle style = TitlebarStyle::DEFAULT);

Ui::Child toolbar(Ui::Children children);

inline Ui::Child toolbar(Meta::Same<Ui::Child> auto... children) {
    return toolbar({children...});
}

Ui::Child bottombar(Ui::Children children);

inline Ui::Child bottombar(Meta::Same<Ui::Child> auto... children) {
    return bottombar({children...});
}

struct Scafold : Meta::NoCopy {
    Mdi::Icon icon;
    String title;
    TitlebarStyle titlebar = TitlebarStyle::DEFAULT;

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
        a.visit(::Visitor{
            [&](ToggleSidebar) {
                s.sidebarOpen = !s.sidebarOpen;
            },
        });
    }

    using Model = Ui::Model<State, Action, reduce>;
};

Ui::Child scafold(Scafold scafold);

} // namespace Hideo
