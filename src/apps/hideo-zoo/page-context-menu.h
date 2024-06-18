#pragma once

#include <karm-kira/context-menu.h>
#include <karm-ui/layout.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_CONTEXT_MENU{
    Mdi::MENU,
    "Context Menu",
    "Displays a menu to the user — such as a set of actions or functions — triggered by a button.",
    [] {
        return Ui::labelMedium("Right click here") |
               Ui::center() |
               Ui::bound() |
               Kr::contextMenu([] {
                   return Kr::contextMenuContent({
                       Kr::contextMenuItem(Ui::NOP, Mdi::OPEN_IN_APP, "Open"),
                       Kr::contextMenuItem(Ui::NOP, Mdi::PENCIL, "Edit"),
                       Ui::separator(),
                       Kr::contextMenuItem(Ui::NOP, Mdi::CONTENT_COPY, "Copy"),
                       Kr::contextMenuItem(Ui::NOP, Mdi::CONTENT_CUT, "Cut"),
                       Kr::contextMenuItem(Ui::NOP, Mdi::CONTENT_PASTE, "Paste"),
                       Ui::separator(),
                       Kr::contextMenuItem(Ui::NOP, Mdi::SHARE, "Interact..."),
                       Kr::contextMenuItem(Ui::NOP, Mdi::CURSOR_TEXT, "Rename..."),
                       Ui::separator(),
                       Kr::contextMenuItem(Ui::NOP, Mdi::DELETE, "Delete"),
                       Ui::separator(),
                       Kr::contextMenuItem(Ui::NOP, Mdi::INFORMATION_OUTLINE, "Properties..."),
                   });
               });
    },
};

} // namespace Hideo::Zoo
