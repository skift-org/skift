#pragma once

#include <karm-kira/side-nav.h>
#include <karm-ui/layout.h>
#include <mdi/dock-left.h>
#include <mdi/duck.h>
#include <mdi/tree.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_SIDENAV{
    Mdi::DOCK_LEFT,
    "Side Navigation"s,
    "A vertical list of links that can be toggled open and closed.",
    [] {
        return Ui::hflow(
            Kr::sidenav({
                Kr::sidenavTitle("Navigation"s),
                Kr::sidenavItem(true, Ui::NOP, Mdi::DUCK, "Item 1"s),
                Kr::sidenavTree(Mdi::TREE, "Item 2"s, [] {
                    return Ui::vflow(
                        8,
                        Kr::sidenavItem(false, Ui::NOP, Mdi::DUCK, "Subitem 1"s),
                        Kr::sidenavItem(false, Ui::NOP, Mdi::DUCK, "Subitem 2"s),
                        Kr::sidenavItem(false, Ui::NOP, Mdi::DUCK, "Subitem 3"s)
                    );
                }),
                Kr::sidenavItem(false, Ui::NOP, Mdi::DUCK, "Item 3"s),
            }),
            Ui::separator()
        );
    },
};

} // namespace Hideo::Zoo
