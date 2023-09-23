#pragma once

#include <karm-main/base.h>

#include "box.h"
#include "dialog.h"
#include "layout.h"
#include "node.h"
#include "reducer.h"

namespace Karm::Ui {

enum struct TitlebarStyle {
    DEFAULT,
    FIXED,
    DIALOG,
};

Child controls(TitlebarStyle style = TitlebarStyle::DEFAULT);

Child titlebar(Mdi::Icon icon, String title, TitlebarStyle style = TitlebarStyle::DEFAULT);

Child titlebar(Mdi::Icon icon, String title, Child tabs, TitlebarStyle style = TitlebarStyle::DEFAULT);

Child toolbar(Children children);

inline Child toolbar(Meta::Same<Child> auto... children) {
    return toolbar({children...});
}

Child bottombar(Children children);

inline Child bottombar(Meta::Same<Child> auto... children) {
    return bottombar({children...});
}

struct Scafold : public Meta::NoCopy {
    Mdi::Icon icon;
    String title;
    TitlebarStyle titlebar = TitlebarStyle::DEFAULT;

    Children startTools;
    Children midleTools;
    Children endTools;

    Opt<Child> sidebar;
    Child body;
    Math::Vec2i size = {800, 600};

    struct State {
        bool sidebarOpen = false;
        bool isMobile = false;
    };

    struct ToggleSidebar {};

    using Action = Var<ToggleSidebar>;

    static void reduce(State &s, Action a) {
        a.visit(::Visitor{
            [&](ToggleSidebar) {
                s.sidebarOpen = !s.sidebarOpen;
                return s;
            },
        });
    }

    using Model = Model<State, Action, reduce>;
};

Child scafold(Scafold scafold);

} // namespace Karm::Ui
