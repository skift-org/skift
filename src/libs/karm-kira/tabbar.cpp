module;

#include <karm-gfx/icon.h>

export module Karm.Kira:tabbar;

import Karm.Ui;
import :separator;

namespace Karm::Kira {

export Ui::Child tabbarWrapper(Ui::Children children) {
    return Ui::hscroll(
        Ui::hflow(4, children)
    );
}

export Ui::Child tabbarContent(Ui::Children children) {
    return Ui::vflow(
        tabbarWrapper(children) | Ui::insets({0, 8}),
        separator(Ui::GRAY900)
    );
}

export Ui::Child tabbarItem(bool selected, Ui::Send<> onSelect, Ui::Child content) {
    content = content | Ui::insets({6, 12}) | Ui::center();

    if (not selected) {
        return Ui::button(
                   onSelect,
                   Ui::ButtonStyle::subtle(),
                   content |
                       Ui::minSize({Ui::UNCONSTRAINED, 36})
               ) |
               Ui::insets({8, 0});
    }

    return Ui::box(
        {
            .margin = {8, 0, 0, 0},
            .padding = {0, 0, 8, 0},
            .borderRadii = {4, 4, 0, 0},
            .backgroundFill = Ui::GRAY900,
        },
        content | Ui::bound()
    );
}

} // namespace Karm::Kira
